#include "stdafx.h"
#include "BombermanScene.h"

#include "ColliderComponent.h"
#include "GameObject.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "RigidBodyComponent.h"
#include"TransformComponent.h"
#include "BombermanCharPrefab.h"
#include "BombermanHelpers.h"
#include "BombManager.h"
#include "ContentManager.h"
#include "CubePrefab.h"
#include "ModelComponent.h"
#include "..\..\Materials/Shadow/DiffuseMaterial_Shadow.h"

BombermanScene::BombermanScene(): GameScene(L"BombermanScene")
{
}

BombermanScene::~BombermanScene()
{
	BombManager::GetInstance()->DestroyInstance();
}

void BombermanScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	const auto gameContext = GetGameContext();
	
	GameObject* pCharacter = new BombermanCharPrefab(m_BlockSize, 2.0f,5.0f,0.01f,50.f);
	pCharacter->GetTransform()->Translate(0,100,0);
	AddChild(pCharacter);
	
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);
	
	//ground (prefab)
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry>geometry(new physx::PxPlaneGeometry());
	ground->AddComponent(
	new ColliderComponent(
		geometry,
		*bouncyMaterial, 
		physx::PxTransform(
			physx::PxQuat(DirectX::XM_PIDIV2,physx::PxVec3(0,0,1))))
	);
	AddChild(ground);

	gameContext.pShadowMapper->SetLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	
	//Level
	/*auto levelGameObject =new GameObject();
	auto* pModelComponent = new ModelComponent(L"Resources/Meshes/Level.ovm");
	levelGameObject->AddComponent(pModelComponent);

	auto* pDiffuseMat = new DiffuseMaterial_Shadow();
	pDiffuseMat->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
	gameContext.pMaterialManager->AddMaterial(pDiffuseMat, 0);
	pDiffuseMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	pModelComponent->SetMaterial(0);
	
	
	auto rb = new RigidBodyComponent(true);
	
	levelGameObject->AddComponent(rb);
	
	physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Level.ovpc");

	std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));
	auto cc= new ColliderComponent(meshGeometry,*bouncyMaterial);
	levelGameObject->AddComponent(cc);*/
	
	//AddChild(levelGameObject);

	CreateFloor(10);
	
}

void BombermanScene::Update()
{
}

void BombermanScene::Draw()
{
}

void BombermanScene::SceneActivated()
{
}

void BombermanScene::SceneDeactivated()
{
}

void BombermanScene::CreateFloor(int const size)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);
	
	int const startX = m_BlockSize/2;
	int const startZ = m_BlockSize/2;
	
	physx::PxVec3 const floorDimension = { static_cast<float>(m_BlockSize),static_cast<float>(m_BlockSize),static_cast<float>(m_BlockSize) };
	std::shared_ptr<physx::PxGeometry> wallGeometry(new physx::PxBoxGeometry(floorDimension / 2.f));
	
	for (int x = 0 ; x < size; ++x)
	{
		int const offsetX = x * m_BlockSize;
		for (int z = 0 ; z < size; ++z)
		{
			int const offsetZ = z * m_BlockSize;
			DirectX::XMFLOAT4 color = {0.f,0.f,0.f,1.f};
			if (x % 2 == 0)
			{
				if (z % 2 == 0)
				color = helpers::RGBADivide(80.f,235.f,75.f,92.f);
				else
				color = helpers::RGBADivide(141.f,212.f,57.f,83.f);
			}
			else
			{
				if (z % 2 == 1)
				color = helpers::RGBADivide(80.f,235.f,75.f,92.f);
			else
				color = helpers::RGBADivide(141.f,212.f,57.f,83.f);
			}
			
			GameObject* pWall = new CubePrefab(floorDimension.x, floorDimension.y, floorDimension.z, color);;
			pWall->GetTransform()->Translate(static_cast<float>(startX + offsetX), 0, static_cast<float>(startZ + offsetZ));

			auto rb = new RigidBodyComponent(true);
			rb->SetCollisionGroup(CollisionGroupFlag::Group0);
			pWall->AddComponent(rb);
			
			auto cc = new ColliderComponent(wallGeometry, *bouncyMaterial);
			pWall->AddComponent(cc);
			AddChild(pWall);
		}
	}
}
