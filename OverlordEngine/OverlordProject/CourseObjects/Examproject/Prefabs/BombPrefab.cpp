#include "stdafx.h"
#include "BombPrefab.h"

#include "../BombermanGameSettings.h"
#include "ColliderComponent.h"
#include "ContentManager.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "../../../Materials/DiffuseMaterial.h"
#include "ModelComponent.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "Explosion.h"
#include "PhysxProxy.h"
#include "BombermanCharPrefab.h"
#include "..//Components/DestroyableWallComponent.h"

BombPrefab::BombPrefab() :GameObject()
{
}

void BombPrefab::Activate(const DirectX::XMFLOAT3& spawnPos)
{
	m_IsActive = true;
	m_IsExploding = false;
	m_pRigidBody->GetTransform()->Translate(spawnPos);
	m_pSmokeEmitter->SetIsActive(true);
}

void BombPrefab::Initialize(const::GameContext& gameContext)
{
	SetTag(BombermanGameSettings::bomb_tag);

	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto pDiffuseMaterial = new DiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Bomb.jpg");
	auto matID = gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial);


	m_pModelComponent = new ModelComponent(L"./Resources/Meshes/Bomb.ovm");
	m_pModelComponent->SetMaterial(matID);

	this->AddComponent(m_pModelComponent);

	auto const material = physx->createMaterial(1.f, 1.f, 0.f);
	m_pRigidBody = new RigidBodyComponent(false);
	m_pRigidBody->SetKinematic(true);
	this->AddComponent(m_pRigidBody);

	physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Bomb.ovpc");

	std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));
	auto const cc = new ColliderComponent(meshGeometry, *material);
	this->AddComponent(cc);

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
	this->AddComponent(m_pSmokeEmitter);
	m_pSmokeEmitter->GetTransform()->Translate(0, 2.5f, 0);

	m_pRigidBody->GetTransform()->Translate(-10000, -10000, -10000);

}

void BombPrefab::PostInitialize(const GameContext&)
{
	m_pRigidBody->GetPxRigidBody()->userData = this;

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
	if (m_IsExploding)
		return;

	m_IsExploding = true;
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


	auto const dir = ToPxVec3(direction);
	auto distance = static_cast<physx::PxReal>(BombermanGameSettings::block_size) - 0.5f;
	auto const pos = ToPxVec3(m_ExplodePos) + distance / 2.f * dir;


	for (physx::PxReal i = 1; i < static_cast<physx::PxReal>(reach) + 1; ++i)  // NOLINT(cert-flp30-c)
	{
		physx::PxHitBuffer<physx::PxRaycastHit> hit{};
		distance = i * BombermanGameSettings::block_size;
		if (!physxProxy->Raycast(pos, dir, distance, hit, physx::PxHitFlag::eDEFAULT))
		{
			CreateExplosion(ToPxVec3(m_ExplodePos) + i * BombermanGameSettings::block_size * dir);
		}
		else
		{
			auto const pOther = static_cast<GameObject*>(hit.getAnyHit(0).actor->userData);
			if (pOther == nullptr)
			{
				Logger::LogError(L"BombPrefab::CreateExplosion, hit RigidBody has no user data!");
				break;
			}
			if (pOther->GetTag() == BombermanGameSettings::bomb_tag)
			{
				Logger::LogInfo(L"Explosion hit other bomb!");
				static_cast<BombPrefab*>(pOther)->Explode();
			}
			if (pOther->GetTag() == BombermanGameSettings::player_tag)
			{
				Logger::LogInfo(L"Explosion hit a player!");
				static_cast<BombermanCharPrefab*>(pOther)->KillPlayer();
			}
			if (pOther->GetTag() == BombermanGameSettings::destructible_tag)
			{
				Logger::LogInfo(L"Explosion hit a destructible!");
				auto* pComp = pOther->GetComponent<DestroyableWallComponent>();
				if (pComp == nullptr)
				{
					Logger::LogError(L"BombPrefab::CreateExplosion, tag is " + BombermanGameSettings::destructible_tag + L", but no DestroyableWallComponent was found");
					break;
				}
				pComp->DestroyObject();
			}
			break;
		}
	}
}

void BombPrefab::CreateExplosion(DirectX::XMFLOAT3 spawnPos)
{
	//Todo Make this more optimized (Object pool)
	auto* pObj = new Explosion();
	pObj->GetTransform()->Translate(spawnPos);
	AddChild(pObj);
}

void BombPrefab::CreateExplosion(physx::PxVec3 spawnPos)
{
	CreateExplosion(ToXMFLOAT3(spawnPos));
}
