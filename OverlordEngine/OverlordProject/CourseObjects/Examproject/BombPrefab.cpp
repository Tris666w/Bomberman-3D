#include "stdafx.h"
#include "BombPrefab.h"

#include "BombermanGameSettings.h"
#include "ColliderComponent.h"
#include "ContentManager.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "../../Materials/DiffuseMaterial.h"
#include "ModelComponent.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "Explosion.h"
#include "PhysxProxy.h"

BombPrefab::BombPrefab() :GameObject()
{
}

void BombPrefab::Activate(const DirectX::XMFLOAT3& spawnPos)
{
	m_IsActive = true;
	m_pRigidBody->GetTransform()->Translate(spawnPos);
	m_pSmokeEmitter->SetIsActive(true);

	Logger::Log(LogLevel::Info, L"Bomb dropped");
}

void BombPrefab::Initialize(const::GameContext& gameContext)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto pDiffuseMaterial = new DiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Bomb.jpg");
	auto matID = gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial);


	m_pModelComponent = new ModelComponent(L"./Resources/Meshes/Bomb.ovm");
	m_pModelComponent->SetMaterial(matID);

	auto obj = new GameObject();
	obj->AddComponent(m_pModelComponent);

	auto const material = physx->createMaterial(1.f, 1.f, 0.f);
	m_pRigidBody = new RigidBodyComponent(false);
	m_pRigidBody->SetKinematic(true);
	obj->AddComponent(m_pRigidBody);

	physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Bomb.ovpc");

	std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));
	auto const cc = new ColliderComponent(meshGeometry, *material);
	obj->AddComponent(cc);

	m_pRigidBody->GetTransform()->Translate(0, 10.f, 0.f);

	//Smoke emitter
	m_pSmokeEmitter = new ParticleEmitterComponent(L"Resources/Textures/smoke_04.png");
	m_pSmokeEmitter->SetShape(EmitterShape::Cone);
	m_pSmokeEmitter->SetStartingColor(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.f));
	m_pSmokeEmitter->SetVelocity(6.f);
	m_pSmokeEmitter->SetMinSize(0.0f);
	m_pSmokeEmitter->SetMaxSize(1.0f);
	m_pSmokeEmitter->SetMinEnergy(1.5f);
	m_pSmokeEmitter->SetMaxEnergy(2.0f);
	m_pSmokeEmitter->SetMinSizeGrow(0.4f);
	m_pSmokeEmitter->SetMaxSizeGrow(1.5f);
	m_pSmokeEmitter->SetMinEmitterRange(0.2f);
	m_pSmokeEmitter->SetMaxEmitterRange(0.5f);
	m_pSmokeEmitter->SetEmitRate(20);
	obj->AddComponent(m_pSmokeEmitter);
	m_pSmokeEmitter->GetTransform()->Translate(0, 2.5f, 0);
	AddChild(obj);
}

void BombPrefab::PostInitialize(const GameContext&)
{
}

void BombPrefab::Draw(const::GameContext&)
{
}

void BombPrefab::Update(const::GameContext& gameContext)
{
	if (!m_IsActive)
		return;

	m_ExplodeTimer += gameContext.pGameTime->GetElapsed();
	if (m_ExplodeTimer >= m_ExplodeTime)
		Explode();


}


void BombPrefab::Explode()
{
	m_IsActive = false;
	m_ExplodeTimer = 0.f;
	m_ExplodePos = m_pRigidBody->GetTransform()->GetWorldPosition();

	CreateExplosion(m_ExplodePos);
	CreateExplosion(DirectX::XMFLOAT3(0, 0, 1), m_ExplosionReach);
	CreateExplosion(DirectX::XMFLOAT3(0, 0, -1), m_ExplosionReach);
	CreateExplosion(DirectX::XMFLOAT3(1, 0, 0), m_ExplosionReach);
	CreateExplosion(DirectX::XMFLOAT3(-1, 0, 0), m_ExplosionReach);

	m_pRigidBody->GetTransform()->Translate(-10000, -10000, -10000);
	m_pSmokeEmitter->SetIsActive(false);

}



void BombPrefab::CreateExplosion(DirectX::XMFLOAT3 direction, int reach)
{
	using namespace DirectX;
	auto const physxProxy = SceneManager::GetInstance()->GetActiveScene()->GetPhysxProxy();


	auto pos = ToPxVec3(m_ExplodePos);
	auto const dir = ToPxVec3(direction);

	auto distance = static_cast<physx::PxReal>(BombermanGameSettings::BlockSize);
	pos = pos + distance * dir;

	for (physx::PxReal i = 1; i < static_cast<physx::PxReal>(reach) + 1; ++i)
	{
		physx::PxHitBuffer<physx::PxRaycastHit> hit{};
		distance = i * BombermanGameSettings::BlockSize;
		if (!physxProxy->Raycast(pos, dir, distance, hit, physx::PxHitFlag::eDEFAULT))
		{
			CreateExplosion(pos + (distance - BombermanGameSettings::BlockSize) * dir);
		}
		else
		{
			Logger::LogInfo(L"Explosion hit something!");
			break;
		}
	}
}

void BombPrefab::CreateExplosion(DirectX::XMFLOAT3 spawnPos)
{
	auto* pObj = new Explosion();
	pObj->GetTransform()->Translate(spawnPos);
	AddChild(pObj);
}

void BombPrefab::CreateExplosion(physx::PxVec3 spawnPos)
{
	CreateExplosion(ToXMFLOAT3(spawnPos));
}
