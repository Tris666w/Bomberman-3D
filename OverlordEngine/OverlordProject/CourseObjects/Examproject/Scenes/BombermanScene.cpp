#include "stdafx.h"
#include "BombermanScene.h"

#include "ColliderComponent.h"
#include "GameObject.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "../Prefabs/BombermanCharPrefab.h"
#include "../BombManager.h"
#include "ContentManager.h"
#include "ModelComponent.h"
#include "PhysxProxy.h"
#include "../Components/LinkGameObjectPosComponent.h"

#include "../Materials\SkyBoxMaterial.h"
#include "../Materials/Post/PostBloom.h"
#include "../../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../../../Materials/Post/PostGrayscale.h"
#include "../../../Materials/Shadow/DiffuseMaterial_Shadow.h"

#include "../BombermanGameSettings.h"
#include "../Components/DestroyableWallComponent.h"

BombermanScene::BombermanScene() : GameScene(L"BombermanScene")
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

	PrintControls();

	//Spawn the camera
	DirectX::XMFLOAT3 const cameraPos = { BombermanGameSettings::grid_size * BombermanGameSettings::block_size / 2.f
									,BombermanGameSettings::grid_size / 3.5f * (BombermanGameSettings::wall_height + 2) * BombermanGameSettings::block_size,
										-BombermanGameSettings::block_size * BombermanGameSettings::grid_size / 2.f };
	DirectX::XMFLOAT3 const cameraRot = { 45,0,0 };
	CreateAndActivateCamera(cameraPos, cameraRot);

	CreatePlayers();

	gameContext.pShadowMapper->SetLight({ -95.6139526f,100,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	CreateFloor(BombermanGameSettings::grid_size);
	CreateWalls(BombermanGameSettings::grid_size, BombermanGameSettings::wall_height);
	CreateSkybox();
	CreateNotDestructibleWalls(BombermanGameSettings::grid_size);
	CreateDestructibleWalls(BombermanGameSettings::grid_size);

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

void BombermanScene::SceneActivated()
{
}

void BombermanScene::SceneDeactivated()
{
}

void BombermanScene::PrintControls() const
{
	Logger::LogInfo(L"LEFT_ARROW to move left");
	Logger::LogInfo(L"RIGHT_ARROW to move left");
	Logger::LogInfo(L"UP_ARROW to move left");
	Logger::LogInfo(L"DOWN_ARROW to move left");
	Logger::LogInfo(L"SPACE to spawn a bomb");

}

void BombermanScene::CreateFloor(int const size)
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
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/WeakBrick.png");
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

			auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Cube.ovm");
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
			
			physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Cube.ovpc");
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

void BombermanScene::CreateAndActivateCamera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot)
{
	auto* pFixedCamera = new CameraComponent();
	auto obj = new GameObject();
	m_pCamera = pFixedCamera;
	obj->AddComponent(pFixedCamera);
	SetActiveCamera(pFixedCamera);
	obj->GetTransform()->Translate(pos);
	obj->GetTransform()->Rotate(rot);
	AddChild(obj);
}

void BombermanScene::CreatePlayers()
{
	std::wstring const meshPath = L"./Resources/Meshes/PeasantGirl.ovm";
	float const playerRadius = BombermanGameSettings::block_size / 2.5f;
	float const playerHeight = 5.0f;
	float const playerStepOffset = 0.01f;
	float const playerMoveSpeed = 25.f;
	
	//Create player1
	std::vector<int> const char1Controls{ 'A', 'D', 'W', 'S', 32 };
	GameObject* pCharacter1 = new BombermanCharPrefab(meshPath, L"./Resources/Textures/PeasantGirl_Diffuse.png",
		playerRadius, playerHeight, playerStepOffset, playerMoveSpeed, char1Controls, GamepadIndex::PlayerOne);

	//Create player2
	std::vector<int> const char2Controls{ VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_RSHIFT };
	GameObject* pCharacter2 = new BombermanCharPrefab(meshPath, L"./Resources/Textures/PeasantGirl_Diffuse.png",
		playerRadius, playerHeight, playerStepOffset, playerMoveSpeed, char2Controls, GamepadIndex::PlayerTwo);

	float const startZ1 = BombermanGameSettings::grid_size * BombermanGameSettings::block_size - BombermanGameSettings::block_size / 2.f;
	float const startX1 = BombermanGameSettings::grid_size * BombermanGameSettings::block_size - BombermanGameSettings::block_size / 2.f;

	AddChild(pCharacter1);
	pCharacter1->GetTransform()->Translate(BombermanGameSettings::block_size / 2.f, 20.f, startZ1);
	AddChild(pCharacter2);
	pCharacter2->GetTransform()->Translate(startX1, 20.f, BombermanGameSettings::block_size / 2.f);
}
