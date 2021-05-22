#include "stdafx.h"
#include "CameraComponent.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "NoiseGenerator.h"


CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(DirectX::XM_PIDIV4),
	m_Size(25.0f),
	m_IsActive(true),
	m_PerspectiveProjection(true),
	m_ShakeStartPos(),
	m_ShakeTimer(0),
	m_ShakeIntensity(0),
	m_IsShaking(false),
	m_ShakeTime(0),
	m_ShakeSpeed(0.2f),
	m_ShakeSeed(rand() % m_MapSize),
	m_Lunacrity(2.f),
	m_Frequency(3.f),
	m_Persistence(0.5f),
	m_MapSize(256),
	m_OctaveCount(8),
	m_ScalingBias(2.f)
{
	XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, DirectX::XMMatrixIdentity());

	m_SeedVector.reserve(m_MapSize);
	m_SeedVector.resize(m_MapSize);
	for (int& seed : m_SeedVector)
		seed = rand() % m_MapSize;

	m_GradientVector.resize(2);
	m_GradientVector[0] = -1.f;
	m_GradientVector[1] = 1.f;

}

void CameraComponent::Initialize(const GameContext&)
{

}

void CameraComponent::Update(const GameContext& gameContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	auto& windowSettings = OverlordGame::GetGameSettings().Window;
	XMMATRIX projection;

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		const float viewHeight = (m_Size > 0) ? m_Size : windowSettings.Height;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const DirectX::XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const DirectX::XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const DirectX::XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const DirectX::XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);

	if (m_IsShaking)
	{
		Shake(gameContext.pGameTime->GetElapsed());
	}
}

void CameraComponent::Draw(const GameContext&) {}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if (gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if (gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups) const
{
	UNREFERENCED_PARAMETER(gameContext);


	float const halfWidth = static_cast<float>(OverlordGame::GetGameSettings().Window.Width) / 2.f;
	float const halfHeight = static_cast<float>(OverlordGame::GetGameSettings().Window.Height) / 2.f;

	float const xPosNdc = (InputManager::GetMousePosition().x - halfWidth) / halfWidth;
	float const yPosNdc = (halfHeight - InputManager::GetMousePosition().y) / halfHeight;

	DirectX::XMFLOAT3 nearPoint = DirectX::XMFLOAT3(xPosNdc, yPosNdc, 0.f);
	DirectX::XMFLOAT3 farPoint = DirectX::XMFLOAT3(xPosNdc, yPosNdc, 1.0f);

	auto nearPointVector = XMLoadFloat3(&nearPoint);
	auto farPointVector = XMLoadFloat3(&farPoint);

	auto const viewProjInv = XMLoadFloat4x4(&m_ViewProjectionInverse);

	nearPointVector = XMVector3TransformCoord(nearPointVector, viewProjInv);
	farPointVector = XMVector3TransformCoord(farPointVector, viewProjInv);


	XMStoreFloat3(&nearPoint, nearPointVector);
	XMStoreFloat3(&farPoint, farPointVector);

	physx::PxVec3 const origin = ToPxVec3(nearPoint);
	physx::PxVec3 const dir = ToPxVec3(DirectX::XMFLOAT3(farPoint.x - nearPoint.x, farPoint.y - nearPoint.y, farPoint.z - nearPoint.z));

	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~static_cast<UINT32>(ignoreGroups);

	physx::PxRaycastBuffer hit;
	if (m_pGameObject->GetScene()->GetPhysxProxy()->Raycast(origin, dir.getNormalized(), PX_MAX_F32, hit, physx::PxHitFlag::eDEFAULT, filterData))
	{
		return static_cast<GameObject*>(hit.getAnyHit(0).actor->userData);
	}

	return nullptr;

}

auto CameraComponent::ShakeCamera(const float duration, float intensity) -> void
{
	m_IsShaking = true;
	m_ShakeIntensity = intensity;
	m_ShakeTime = duration;
	m_ShakeTimer = 0;
	m_ShakeStartPos = GetTransform()->GetWorldPosition();
}

void CameraComponent::Shake(float const elapsedSec)
{
	m_ShakeTimer += elapsedSec;
	if (m_ShakeTimer >= m_ShakeTime)
	{
		GetTransform()->Translate(m_ShakeStartPos);
		m_IsShaking = false;
		return;
	}
	auto& pos = m_ShakeStartPos;

	float x = NoiseGenerator::PerlinNoise1D(m_ShakeSeed, m_Frequency * elapsedSec, m_OctaveCount, m_Lunacrity, m_Persistence, m_SeedVector, m_GradientVector);
	float y = NoiseGenerator::PerlinNoise1D(m_ShakeSeed + 1, m_Frequency * elapsedSec, m_OctaveCount, m_Lunacrity, m_Persistence, m_SeedVector, m_GradientVector);
	float z = NoiseGenerator::PerlinNoise1D(m_ShakeSeed + 2, m_Frequency * elapsedSec, m_OctaveCount, m_Lunacrity, m_Persistence, m_SeedVector, m_GradientVector);
	DirectX::XMFLOAT3 const newPos =
	{
		pos.x + m_ShakeIntensity * x,
		pos.y + m_ShakeIntensity * y,
		pos.z + m_ShakeIntensity * z

	};
	GetTransform()->Translate(newPos);
}