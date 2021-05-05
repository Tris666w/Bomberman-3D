#include "stdafx.h"
#include "BombermanScene.h"

#include "GameObject.h"
#include "PhysxProxy.h"
#include "PhysxManager.h"
#include "ContentManager.h"
#include "ModelComponent.h"
#include "ColliderComponent.h"
#include "OverlordGame.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"

#include "../BombManager.h"
#include "../BombermanUI.h"
#include "../BombermanGameSettings.h"
#include "../Prefabs/BombermanCharPrefab.h"
#include "../Components/DestroyableWallComponent.h"

#include "../Materials/GrassMaterial.h"
#include "../Materials/SkyBoxMaterial.h"
#include "../Materials/Post/PostBloom.h"
#include "../../../Materials/Post/PostGrayscale.h"
#include "../../../Materials/Shadow/DiffuseMaterial_Shadow.h"

BombermanScene::BombermanScene() : GameScene(L"BombermanScene")
{
}

BombermanScene::~BombermanScene()
{
	BombManager::GetInstance()->DestroyInstance();
}

void BombermanScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(false);
	const auto gameContext = GetGameContext();


	//Spawn the camera
	DirectX::XMFLOAT3 const cameraPos = { BombermanGameSettings::grid_size * BombermanGameSettings::block_size / 1.98f
									,BombermanGameSettings::grid_size / 3.f * (BombermanGameSettings::wall_height + 2) * BombermanGameSettings::block_size,
										-BombermanGameSettings::block_size * BombermanGameSettings::grid_size /3.f };
	DirectX::XMFLOAT3 const cameraRot = { 60,0,0 };
	CreateAndActivateCamera(cameraPos, cameraRot);


	gameContext.pShadowMapper->SetLight({ -100,150,-50 }, { 0.740129888f, -0.597205281f, 0.309117377f });

	CreatePlayers();
	CreateLevel();

	auto const bloomPp = new PostBloom();
	bloomPp->SetIntensity(0.35f);
	AddPostProcessingEffect(bloomPp);
}

void BombermanScene::Update()
{
}

void BombermanScene::Draw()
{
}

void BombermanScene::CreateLevel()
{
	CreateGridFloor(BombermanGameSettings::grid_size);
	CreateWalls(BombermanGameSettings::grid_size, BombermanGameSettings::wall_height);
	CreateNotDestructibleWalls(BombermanGameSettings::grid_size);
	CreateDestructibleWalls(BombermanGameSettings::grid_size);
	CreateSkybox();
	CreateFloor();
	//CreateGrass();
	
}

void BombermanScene::CreateGrass()
{
	auto const pObj = new GameObject();
	auto const pModel = new ModelComponent(L"Resources/Meshes/Plane.ovm");
	auto const pMat = new GrassMaterial(L"Resources/Effects/Grass.fx");
	auto const matId = GetGameContext().pMaterialManager->AddMaterial(pMat);
	pModel->SetMaterial(matId);
	pObj->AddComponent(pModel);
	pMat->SetDiffuseTexture(L"Resources/Textures/Grass/Diffuse.png");
	pMat->SetHeightTexture(L"Resources/Textures/Grass/Height.png");
	pMat->SetWeightTexture(L"Resources/Textures/Grass/Weight.png");
	pMat->SetAddedColorTexture(L"Resources/Textures/Grass/AddColor.png");
	pMat->SetDirectionTexture(L"Resources/Textures/Grass/Direction.png");
	pMat->SetLightDirection( GetGameContext().pShadowMapper->GetLightDirection());
	pObj->GetTransform()->Translate(25,0,0);
	AddChild(pObj);
}

void BombermanScene::CreateGridFloor(int const size)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);

	auto const blockSize = BombermanGameSettings::block_size;

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

void BombermanScene::CreateFloor()
{
	auto const pObj = new GameObject();
	auto const pModel = new ModelComponent(L"Resources/Meshes/LevelFloor.ovm");
	auto const pMat = new DiffuseMaterial_Shadow();
	pMat->SetDiffuseTexture(L"Resources/Textures/FloorTexture.jpg");
	auto const matId = GetGameContext().pMaterialManager->AddMaterial(pMat);
	pObj->AddComponent(pModel);
	pModel->SetMaterial(matId);
	AddChild(pObj);
	pObj->GetTransform()->Translate(31,0,50);
	
}

void BombermanScene::CreateWalls(int const size, int const wallHeight)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);

	float const blockSize = BombermanGameSettings::block_size;

	float startX = -blockSize / 2.f;
	float startZ = -blockSize / 2.f;

	auto pDiffuseMaterial = new DiffuseMaterial_Shadow();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Wall.png");
	auto const matId = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial);

	for (int z = 0; z < 2; ++z)
	{
		for (int y = 0; y < wallHeight; ++y)
		{
			for (int x = 0; x < size + 2; ++x)
			{
				float const offsetX = static_cast<float>(x) * blockSize;
				auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Cube.ovm");
				pModelComponent->SetMaterial(matId);

				auto* pWall = new GameObject();
				pWall->AddComponent(pModelComponent);

				pWall->GetTransform()->Translate(startX + offsetX, static_cast<float>(y) * blockSize, startZ);

				auto const rb = new RigidBodyComponent(true);
				rb->SetCollisionGroup(CollisionGroupFlag::Group0);
				pWall->AddComponent(rb);

				physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Cube.ovpc");
				std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));

				auto const cc = new ColliderComponent(meshGeometry, *bouncyMaterial);
				pWall->AddComponent(cc);
				AddChild(pWall);

			}
		}
		startZ += (static_cast<float>(size) + 1.f) * blockSize;
	}

	startZ = blockSize / 2;
	for (int x = 0; x < 2; ++x)
	{
		for (int y = 0; y < wallHeight; ++y)
		{
			for (int z = 0; z < size; ++z)
			{
				float const offsetZ = z * blockSize;
				auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Cube.ovm");
				pModelComponent->SetMaterial(matId);

				auto* pWall = new GameObject();
				pWall->AddComponent(pModelComponent);

				pWall->GetTransform()->Translate(startX, static_cast<float>(y) * blockSize, startZ + offsetZ);

				auto const rb = new RigidBodyComponent(true);
				rb->SetCollisionGroup(CollisionGroupFlag::Group0);
				pWall->AddComponent(rb);
				physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Cube.ovpc");
				std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));

				auto const cc = new ColliderComponent(meshGeometry, *bouncyMaterial);
				pWall->AddComponent(cc);
				AddChild(pWall);
			}
		}
		startX += (size + 1) * blockSize;
	}
}

void BombermanScene::CreateNotDestructibleWalls(int const size)
{
	if (size <= 5)
	{
		Logger::LogWarning(L"Size can't be less than 5");
		return;
	}
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);

	auto const blockSize = BombermanGameSettings::block_size;

	int const startX = blockSize / 2;
	int const startZ = blockSize / 2;

	auto pDiffuseMaterial = new DiffuseMaterial_Shadow();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Wall.png");
	auto const matId = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial);

	for (int x = 1; x < size - 1; x += 2)
	{
		int const offsetX = x * blockSize;
		for (int z = 1; z < size - 1; z += 2)
		{
			int const offsetZ = z * blockSize;

			auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Cube.ovm");
			pModelComponent->SetMaterial(matId);

			auto* pWall = new GameObject();
			pWall->AddComponent(pModelComponent);

			pWall->GetTransform()->Translate(static_cast<float>(startX + offsetX), BombermanGameSettings::block_size, static_cast<float>(startZ + offsetZ));

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

void BombermanScene::CreateDestructibleWalls(int const size)
{
	if (size <= 5)
	{
		Logger::LogWarning(L"Size can't be less than 5");
		return;
	}
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);

	auto const blockSize = BombermanGameSettings::block_size;

	int const startX = blockSize / 2;
	int const startZ = blockSize / 2;

	auto pDiffuseMaterial = new DiffuseMaterial_Shadow();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Barrel.png");
	auto const matId = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial);

	for (int x = 0; x < size; ++x)
	{
		int const offsetX = x * blockSize;
		for (int z = 0; z < size; ++z)
		{
			int const offsetZ = z * blockSize;

			//Places for static walls
			if (x % 2 == 1 && z % 2 == 1)
			{
				continue;
			}

			if ((x == 0 || x == size - 1) && (z < 2 || z > size - 3))
			{
				continue;
			}

			if ((z == 0 || z == size - 1) && (x < 2 || x > size - 3))
			{
				continue;
			}

			//Use spawnPercent
			if (!(static_cast<float>(rand() % 100) < BombermanGameSettings::breakable_wall_spawn_percentage * 100))
				continue;

			auto* pWall = new GameObject();

			auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Barrel.ovm");
			pModelComponent->SetMaterial(matId);
			pWall->AddComponent(pModelComponent);

			pWall->GetTransform()->Translate(static_cast<float>(startX + offsetX), BombermanGameSettings::block_size, static_cast<float>(startZ + offsetZ));

			auto const dc = new DestroyableWallComponent();
			pWall->AddComponent(dc);
			pWall->SetTag(BombermanGameSettings::destructible_tag);

			auto const rb = new RigidBodyComponent();
			rb->SetKinematic(true);
			rb->SetCollisionGroup(CollisionGroupFlag::Group0);
			pWall->AddComponent(rb);
			
			physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Barrel.ovpc");
			std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));

			auto const cc = new ColliderComponent(meshGeometry, *bouncyMaterial);
			pWall->AddComponent(cc);
			AddChild(pWall);
			
			rb->GetPxRigidBody()->userData = pWall;
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

	//Create and assign material
	auto* pDiffuseMat = new SkyBoxMaterial();
	pDiffuseMat->SetCubeMapTexture(L"Resources/Textures/Skybox/Night.dds");
	auto const matID = gameContext.pMaterialManager->AddMaterial(pDiffuseMat);
	pModelComponent->SetMaterial(matID);

	AddChild(skyboxGameObject);
	skyboxGameObject->GetTransform()->Translate(0,-250,0);
}

void BombermanScene::CreateAndActivateCamera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot)
{
	auto* pFixedCamera = new CameraComponent();
	auto obj = new GameObject();
	m_pCamera = pFixedCamera;
	obj->AddComponent(pFixedCamera);
	SetActiveCamera(pFixedCamera);
	obj->GetTransform()->Translate(pos);
	obj->GetTransform()->Rotate(rot);
	//pFixedCamera->SetFieldOfView(1.f);
	AddChild(obj);
}

void BombermanScene::CreatePlayers()
{
	auto const pWindow =  OverlordGame::GetGameSettings().Window;
	
	std::wstring const meshPath = L"./Resources/Meshes/PeasantGirl.ovm";
	float const playerRadius = BombermanGameSettings::block_size / 2.5f;
	float const playerHeight = 5.0f;
	float const playerStepOffset = 0.01f;
	float const playerMoveSpeed = 25.f;

	float const uiNearXPos = pWindow.Width / 30.f;
	float const uiNearYPos = pWindow.Height / 20.f;	
	float const uiFarXPos = static_cast<float>(pWindow.Width) - pWindow.Width / 8.f;
	float const uiFarYPos = static_cast<float>(pWindow.Height) - pWindow.Height / 10.f;	

	float const startX = BombermanGameSettings::grid_size * BombermanGameSettings::block_size - BombermanGameSettings::block_size / 2.f;
	float const startY = BombermanGameSettings::block_size * 1.5f;
	float const startZ = BombermanGameSettings::grid_size * BombermanGameSettings::block_size - BombermanGameSettings::block_size / 2.f;
	
	//Create player 1
	std::vector<int> const char1Controls{ 'A', 'D', 'W', 'S', 32 };
	auto* pCharacter1 = new BombermanCharPrefab(meshPath, L"./Resources/Textures/PeasantGirl_Diffuse.png",
		playerRadius, playerHeight, playerStepOffset, playerMoveSpeed, char1Controls, GamepadIndex::PlayerOne);
	AddChild(pCharacter1);
	pCharacter1->GetTransform()->Translate(BombermanGameSettings::block_size / 2.f, startY, startZ);
	auto* pUi = new BombermanUi(pCharacter1,DirectX::XMFLOAT2{uiNearXPos,uiNearYPos});
	AddChild(pUi);
	
	//Create player 2
	std::vector<int> const char2Controls{ VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_RSHIFT };
	auto* pCharacter2 = new BombermanCharPrefab(meshPath, L"./Resources/Textures/PeasantGirl_Diffuse.png",
		playerRadius, playerHeight, playerStepOffset, playerMoveSpeed, char2Controls, GamepadIndex::PlayerTwo);
	AddChild(pCharacter2);
	pCharacter2->GetTransform()->Translate(startX, startY, startZ);
	pUi = new BombermanUi(pCharacter2,DirectX::XMFLOAT2{static_cast<float>(uiFarXPos),uiNearYPos});
	AddChild(pUi);

	//Create player 3
	std::vector<int> const char3Controls{ VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_RSHIFT };
	auto* pCharacter3 = new BombermanCharPrefab(meshPath, L"./Resources/Textures/PeasantGirl_Diffuse.png",
		playerRadius, playerHeight, playerStepOffset, playerMoveSpeed, char3Controls, GamepadIndex::PlayerThree);
	AddChild(pCharacter3);
	pCharacter3->GetTransform()->Translate(startX, startY, BombermanGameSettings::block_size / 2.f);
	pUi = new BombermanUi(pCharacter3,DirectX::XMFLOAT2{uiNearXPos, static_cast<float>(uiFarYPos)},true);
	AddChild(pUi);

	//Create player 4
	std::vector<int> const char4Controls{ VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_RSHIFT };
	auto* pCharacter4 = new BombermanCharPrefab(meshPath, L"./Resources/Textures/PeasantGirl_Diffuse.png",
		playerRadius, playerHeight, playerStepOffset, playerMoveSpeed, char4Controls, GamepadIndex::PlayerFour);
	AddChild(pCharacter4);
	pCharacter4->GetTransform()->Translate(BombermanGameSettings::block_size / 2.f, startY, BombermanGameSettings::block_size / 2.f);
	pUi = new BombermanUi(pCharacter4,DirectX::XMFLOAT2{static_cast<float>(uiFarXPos),static_cast<float>(uiFarYPos)},true);
	AddChild(pUi);
}
