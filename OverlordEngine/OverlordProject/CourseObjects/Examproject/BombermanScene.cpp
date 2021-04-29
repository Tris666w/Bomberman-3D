#include "stdafx.h"
#include "BombermanScene.h"

#include "ColliderComponent.h"
#include "GameObject.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "RigidBodyComponent.h"
#include"TransformComponent.h"
#include "BombermanCharPrefab.h"
#include "ContentManager.h"
#include "ModelComponent.h"
#include "..\..\Materials/Shadow/DiffuseMaterial_Shadow.h"

BombermanScene::BombermanScene(): GameScene(L"BombermanScene")
{
}

BombermanScene::~BombermanScene()
{
}

void BombermanScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	const auto gameContext = GetGameContext();
	
	GameObject* pCharacter = new BombermanCharPrefab();
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
	auto levelGameObject =new GameObject();
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
	levelGameObject->AddComponent(cc);
	
	AddChild(levelGameObject);
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
