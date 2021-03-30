#include "stdafx.h"
#include "CameraComponent.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "SceneManager.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(DirectX::XM_PIDIV4),
	m_Size(25.0f),
	m_IsActive(true),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, DirectX::XMMatrixIdentity());
}

void CameraComponent::Initialize(const GameContext&) {}

void CameraComponent::Update(const GameContext&)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	DirectX::XMMATRIX projection;

	if (m_PerspectiveProjection)
	{
		projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		const float viewHeight = (m_Size > 0) ? m_Size : windowSettings.Height;
		projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
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


	float const halfWidth = static_cast<float>(OverlordGame::GetGameSettings().Window.Width) /2.f;
	float const halfHeight = static_cast<float>(OverlordGame::GetGameSettings().Window.Height) /2.f;
	
	float const xPosNdc = (InputManager::GetMousePosition().x - halfWidth) / halfWidth;
	float const yPosNdc = (halfHeight - InputManager::GetMousePosition().y) / halfHeight;
	
	DirectX::XMFLOAT3 nearPoint = DirectX::XMFLOAT3(xPosNdc,yPosNdc,0.f);
	DirectX::XMFLOAT3 farPoint = DirectX::XMFLOAT3(xPosNdc,yPosNdc,1.0f);
	
	auto nearPointVector = XMLoadFloat3(&nearPoint);
	auto farPointVector = XMLoadFloat3(&farPoint);

	auto const viewProjInv = XMLoadFloat4x4(&m_ViewProjectionInverse);
	
	nearPointVector = XMVector3TransformCoord(nearPointVector,viewProjInv);
	farPointVector = XMVector3TransformCoord(farPointVector,viewProjInv);
	

	XMStoreFloat3(&nearPoint, nearPointVector);
	XMStoreFloat3(&farPoint, farPointVector);
	
	physx::PxVec3 const origin = ToPxVec3(nearPoint);
	physx::PxVec3 const dir = ToPxVec3(DirectX::XMFLOAT3(farPoint.x - nearPoint.x,farPoint.y-nearPoint.y,farPoint.z-nearPoint.z));
	
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~static_cast<UINT32>(ignoreGroups);
	
	physx::PxRaycastBuffer hit;
	if (m_pGameObject->GetScene()->GetPhysxProxy()->Raycast(origin,dir.getNormalized(),PX_MAX_F32,hit, physx::PxHitFlag::eDEFAULT,filterData))
	{
		return static_cast<GameObject*>(hit.block.actor->userData);	
	}
	
	return nullptr;
	
}