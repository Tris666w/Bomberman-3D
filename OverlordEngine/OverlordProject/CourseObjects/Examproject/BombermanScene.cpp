#include "stdafx.h"
#include "BombermanScene.h"

#include "ColliderComponent.h"
#include "GameObject.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "RigidBodyComponent.h"
#include"TransformComponent.h"
#include "BombermanCharPrefab.h"
#include "BombManager.h"
#include "ContentManager.h"
#include "ModelComponent.h"
#include "LinkGameObjectPosComponent.h"
#include "..\..\Materials\SkyBoxMaterial.h"
#include "..\..\Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "..\..\Materials/Post/PostBloom.h"
#include "..\..\Materials/Post/PostGrayscale.h"

#include "..\..\Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "BombermanGameSettings.h"

BombermanScene::BombermanScene() : GameScene(L"BombermanScene")
{
}

BombermanScene::~BombermanScene()
{
	BombManager::GetInstance()->DestroyInstance();
}

void BombermanScene::Initialize()
{

	const auto gameContext = GetGameContext();
	
	GameObject* pCharacter = new BombermanCharPrefab(L"./Resources/Meshes/PeasantGirl.ovm", L"./Resources/Textures/PeasantGirl_Diffuse.png",
		2.0f, 5.0f, 0.01f, 50.f);
	pCharacter->GetTransform()->Translate(0, 100, 0);
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
				physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1))))
	);
	AddChild(ground);

	gameContext.pShadowMapper->SetLight({ -95.6139526f,100,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	CreateFloor(10);
	CreateSkybox();

	auto const bloomPP = new PostBloom();
	bloomPP->SetIntensity(0.35f);
	AddPostProcessingEffect(bloomPP);

	
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

	auto const blockSize = BombermanGameSettings::BlockSize;

	int const startX = blockSize / 2;
	int const startZ = blockSize / 2;

	auto pDiffuseMaterial1 = new DiffuseMaterial_Shadow();
	pDiffuseMaterial1->SetDiffuseTexture(L"./Resources/Textures/Floor1.png");
	auto const matID1 = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial1);

	auto pDiffuseMaterial2 = new DiffuseMaterial_Shadow();
	pDiffuseMaterial2->SetDiffuseTexture(L"./Resources/Textures/Floor2.png");
	auto const matID2 = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial2);


	for (int x = 0; x < size; ++x)
	{
		int const offsetX = x * blockSize;
		for (int z = 0; z < size; ++z)
		{
			auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Cube.ovm");

			int const offsetZ = z * blockSize;
			if (x % 2 == 0)
			{
				if (z % 2 == 0)
					pModelComponent->SetMaterial(matID1);
				else
					pModelComponent->SetMaterial(matID2);
			}
			else
			{
				if (z % 2 == 1)
					pModelComponent->SetMaterial(matID1);

				else
					pModelComponent->SetMaterial(matID2);
			}

			auto* pWall = new GameObject();
			pWall->AddComponent(pModelComponent);

			pWall->GetTransform()->Translate(static_cast<float>(startX + offsetX), 0, static_cast<float>(startZ + offsetZ));

			auto rb = new RigidBodyComponent(true);
			rb->SetCollisionGroup(CollisionGroupFlag::Group0);
			pWall->AddComponent(rb);

			physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Cube.ovpc");
			std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));

			auto cc = new ColliderComponent(meshGeometry, *bouncyMaterial);
			pWall->AddComponent(cc);
			AddChild(pWall);

		}
	}
}

void BombermanScene::CreateSkybox()
{
	const auto gameContext = GetGameContext();

	//Create game object and model
	auto const skyboxGameObject = new GameObject();
	auto* const pModelComponent = new ModelComponent(L"Resources/Meshes/Sphere.ovm");
	skyboxGameObject->AddComponent(pModelComponent);

	//Link geometry to camera (position) and scale the mesh
	auto* linkComp = new LinkGameObjectPosComponent(gameContext.pCamera->GetGameObject());
	skyboxGameObject->AddComponent(linkComp);
	skyboxGameObject->GetTransform()->Scale(10, 20, 10);

	//Create and assign material
	auto* pDiffuseMat = new SkyBoxMaterial();
	pDiffuseMat->SetCubeMapTexture(L"Resources/Textures/Skybox/Night.dds");
	auto const matID = gameContext.pMaterialManager->AddMaterial(pDiffuseMat);
	pModelComponent->SetMaterial(matID);

	AddChild(skyboxGameObject);
}
