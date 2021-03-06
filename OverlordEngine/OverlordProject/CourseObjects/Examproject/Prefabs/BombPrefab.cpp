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
#include "SoundManager.h"
#include "StumpPrefab.h"
#include "../ExplosionManager.h"

BombPrefab::BombPrefab() :GameObject(),
m_ExplosionReach()
{
}

BombPrefab::~BombPrefab()
{

}

void BombPrefab::Activate(const DirectX::XMFLOAT3& spawnPos, int reach)
{
	m_ExplosionReach = reach;
	m_IsActive = true;
	m_IsExploding = false;
	m_pRigidBody->GetTransform()->Translate(spawnPos);
	m_pSmokeEmitter->SetEmissionActive(true);
}

void BombPrefab::Initialize(const::GameContext& gameContext)
{
	SetTag(BombermanGameSettings::GetInstance()->GetBombTag());

	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto pDiffuseMaterial = new DiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Bomberman/Bomb.jpg");
	auto const matID = gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial);

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
	m_pSmokeEmitter = new ParticleEmitterComponent(L"Resources/Textures/Particles/smoke_04.png");
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

	//Sound
	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/Explosion.wav", FMOD_DEFAULT, 0, &m_pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	m_pSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);

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
	{
		gameContext.pCamera->ShakeCamera(0.5f, 1.f);
		Explode();
	}
}


void BombPrefab::Explode()
{
	if (m_IsExploding)
		return;

	SoundManager::GetInstance()->GetSystem()->playSound(m_pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetSoundVolume());

	m_IsExploding = true;
	m_IsActive = false;
	m_ExplodeTimer = 0.f;
	m_ExplodePos = m_pRigidBody->GetTransform()->GetWorldPosition();
	m_pRigidBody->GetTransform()->Translate(-10000, -10000, -10000);

	CreateExplosion(m_ExplodePos);
	CreateExplosion(DirectX::XMFLOAT3(0, 0, 1), m_ExplosionReach);
	CreateExplosion(DirectX::XMFLOAT3(0, 0, -1), m_ExplosionReach);
	CreateExplosion(DirectX::XMFLOAT3(1, 0, 0), m_ExplosionReach);
	CreateExplosion(DirectX::XMFLOAT3(-1, 0, 0), m_ExplosionReach);

	m_pSmokeEmitter->SetEmissionActive(false);
}



void BombPrefab::CreateExplosion(DirectX::XMFLOAT3 direction, int reach)
{
	using namespace DirectX;
	auto const physxProxy = SceneManager::GetInstance()->GetActiveScene()->GetPhysxProxy();


	auto blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();
	auto const dir = ToPxVec3(direction);
	auto distance = static_cast<physx::PxReal>(blockSize) - 0.5f;
	auto const pos = ToPxVec3(m_ExplodePos) + distance / 2.f * dir;


	for (physx::PxReal i = 1; i < static_cast<physx::PxReal>(reach) + 1; ++i)
	{
		physx::PxHitBuffer<physx::PxRaycastHit> hit{};
		distance = i * blockSize;
		if (!physxProxy->Raycast(pos, dir, distance, hit, physx::PxHitFlag::eDEFAULT))
		{
			CreateExplosion(ToPxVec3(m_ExplodePos) + i * blockSize * dir);
		}
		else
		{
			auto const pOther = reinterpret_cast<BaseComponent*>(hit.getAnyHit(0).shape->getActor()->userData)->GetGameObject();
			if (pOther == nullptr)
			{
				Logger::LogError(L"BombPrefab::CreateExplosion, hit RigidBody has no user data!");
				break;
			}
			if (pOther->GetTag() == BombermanGameSettings::GetInstance()->GetBombTag())
			{
				static_cast<BombPrefab*>(pOther)->Explode();
			}
			if (pOther->GetTag() == BombermanGameSettings::GetInstance()->GetPlayerTag())
			{
				static_cast<BombermanCharPrefab*>(pOther)->DamagePlayer(m_Damage);
			}
			if (pOther->GetTag() == BombermanGameSettings::GetInstance()->GetDestructibleTag())
			{
				static_cast<StumpPrefab*>(pOther)->Break();
			}
			break;
		}
	}
}

void BombPrefab::CreateExplosion(DirectX::XMFLOAT3 spawnPos)
{
	auto* pObj = ExplosionManager::GetInstance()->GetExplosion();
	if (pObj)
	{
		pObj->GetTransform()->Translate(spawnPos);
		pObj->SetIsEnabled(true);
	}
	else
	{
		pObj = new Explosion();
		AddChild(pObj);
		pObj->GetTransform()->Translate(spawnPos);
		pObj->SetIsEnabled(true);
	}

}

void BombPrefab::CreateExplosion(const physx::PxVec3& spawnPos)
{
	CreateExplosion(ToXMFLOAT3(spawnPos));
}
