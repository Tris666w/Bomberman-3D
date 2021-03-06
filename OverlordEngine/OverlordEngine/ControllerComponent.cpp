#include "stdafx.h"
#include "ControllerComponent.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "SceneManager.h"

#pragma warning(push)
#pragma warning(disable: 26812)
ControllerComponent::ControllerComponent(physx::PxMaterial* material, float radius, float height, float stepOffset, std::wstring name,
	physx::PxCapsuleClimbingMode::Enum climbingMode)
	: m_Radius(radius),
	m_Height(height),
	m_StepOffset(stepOffset),
	m_Name(std::move(name)),
	m_Controller(nullptr),
	m_ClimbingMode(climbingMode),
	m_pMaterial(material),
	m_CollisionFlag(physx::PxControllerCollisionFlags()),
	m_CollisionGroups(physx::PxFilterData(static_cast<UINT32>(CollisionGroupFlag::Group0), 0, 0, 0))
{
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4244)
void ControllerComponent::Initialize(const GameContext&)
{
	if (m_Controller != nullptr)
	{
		Logger::LogError(L"[ControllerComponent] Cannot initialize a controller twice");
		return;
	}

	auto* pControllerManager = m_pGameObject->GetScene()->GetPhysxProxy()->GetControllerManager();

	physx::PxCapsuleControllerDesc capsuleControllerDesc{};
	capsuleControllerDesc.setToDefault();

	capsuleControllerDesc.radius = m_Radius;
	capsuleControllerDesc.height = m_Height;
	capsuleControllerDesc.climbingMode = m_ClimbingMode;
	capsuleControllerDesc.contactOffset = 0.1f;
	capsuleControllerDesc.stepOffset = m_StepOffset;
	capsuleControllerDesc.upDirection = physx::PxVec3(0.f, 1.f, 0.f);
	capsuleControllerDesc.material = m_pMaterial;
	capsuleControllerDesc.position = ToPxExtendedVec3(GetGameObject()->GetTransform()->GetPosition());
	capsuleControllerDesc.userData = this;

	m_Controller = pControllerManager->createController(capsuleControllerDesc);
	if (m_Controller == nullptr)
	{
		Logger::LogError(L"[ControllerComponent] Failed to create controller");
		return;
	}


	std::string const name = std::string(m_Name.begin(), m_Name.end());
	m_Controller->getActor()->setName(name.c_str());
	m_Controller->getActor()->userData = this;

	m_Controller->setUserData(this);
	SetCollisionGroup(static_cast<CollisionGroupFlag>(m_CollisionGroups.word0));
	SetCollisionIgnoreGroups(static_cast<CollisionGroupFlag>(m_CollisionGroups.word1));
}
#pragma warning(pop)

void ControllerComponent::Update(const GameContext&)
{
}

void ControllerComponent::Draw(const GameContext&)
{
}

void ControllerComponent::Translate(const DirectX::XMFLOAT3& position) const
{
	Translate(position.x, position.y, position.z);
}

void ControllerComponent::Translate(const float x, const float y, const float z) const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Translate. No controller present");
	else
		m_Controller->setPosition(physx::PxExtendedVec3(x, y, z));
}

void ControllerComponent::Move(const DirectX::XMFLOAT3 displacement, const float minDist)
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot Move. No controller present");
	else
		m_CollisionFlag = m_Controller->move(ToPxVec3(displacement), minDist, 0, nullptr, nullptr);
}

DirectX::XMFLOAT3 ControllerComponent::GetPosition() const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get position. No controller present");
	else
		return ToXMFLOAT3(m_Controller->getPosition());

	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 ControllerComponent::GetFootPosition() const
{
	if (m_Controller == nullptr)
		Logger::LogError(L"[ControllerComponent] Cannot get footposition. No controller present");
	else
		return ToXMFLOAT3(m_Controller->getFootPosition());

	return DirectX::XMFLOAT3();
}

void ControllerComponent::SetCollisionIgnoreGroups(const CollisionGroupFlag ignoreGroups)
{
	using namespace physx;

	m_CollisionGroups.word1 = static_cast<PxU32>(ignoreGroups);

	if (m_Controller != nullptr)
	{
		const auto actor = m_Controller->getActor();
		const auto numShapes = actor->getNbShapes();
		const auto shapes = new PxShape * [numShapes];

		const auto numPointers = actor->getShapes(shapes, numShapes);
		for (PxU32 i = 0; i < numPointers; i++)
		{
#pragma warning (push)
#pragma warning (disable: 6385)
			auto shape = shapes[i];
#pragma warning (pop)
			shape->setSimulationFilterData(m_CollisionGroups);
		}
		delete[] shapes;
	}
}

void ControllerComponent::SetCollisionGroup(const CollisionGroupFlag group)
{
	using namespace physx;

	m_CollisionGroups.word0 = static_cast<UINT32>(group);

	if (m_Controller != nullptr)
	{
		const auto actor = m_Controller->getActor();
		const auto numShapes = actor->getNbShapes();
		const auto shapes = new PxShape * [numShapes];

		const auto numPointers = actor->getShapes(shapes, numShapes);
		for (PxU32 i = 0; i < numPointers; i++)
		{
#pragma warning (push)
#pragma warning (disable: 6385)
			auto shape = shapes[i];
#pragma warning (pop)
			shape->setSimulationFilterData(m_CollisionGroups);
			shape->setQueryFilterData(m_CollisionGroups);
		}
		delete[] shapes;
	}
}
