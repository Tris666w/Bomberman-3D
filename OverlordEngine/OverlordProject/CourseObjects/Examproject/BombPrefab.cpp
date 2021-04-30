#include "stdafx.h"
#include "BombPrefab.h"

#include "ColliderComponent.h"
#include "ContentManager.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "../../Materials/DiffuseMaterial.h"
#include "ModelComponent.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"

BombPrefab::BombPrefab():GameObject()
{
}

void BombPrefab::Activate(const DirectX::XMFLOAT3& spawnPos)
{
	m_IsActive = true;
	m_pRigigdBody->GetTransform()->Translate(spawnPos);

	Logger::Log(LogLevel::Info,L"Bomb dropped");
}

void BombPrefab::Initialize(const::GameContext& gameContext)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto pDiffuseMaterial = new DiffuseMaterial();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Bomb.jpg");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMaterial, 2);
	

	m_pModelComponent = new ModelComponent(L"./Resources/Meshes/Bomb.ovm");
	m_pModelComponent->SetMaterial(2);
	
	auto obj = new GameObject();
	obj->AddComponent(m_pModelComponent);

	auto const material = physx->createMaterial(1.f,1.f,0.f);
	m_pRigigdBody = new RigidBodyComponent(false);
	m_pRigigdBody->SetKinematic(true);
	obj->AddComponent(m_pRigigdBody);
	
	physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Bomb.ovpc");

	std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));
	auto const cc= new ColliderComponent(meshGeometry,*material);
	obj->AddComponent(cc);
	AddChild(obj);

	m_pRigigdBody->GetTransform()->Translate(0,10.f,0.f);
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
	Logger::Log(LogLevel::Info,L"Bomb exploded");
}
