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
#include "SceneManager.h"
#include "SoundManager.h"
#include "TransformComponent.h"

#include "../BombManager.h"
#include "../BombermanUI.h"
#include "../BombermanGameSettings.h"
#include "../Prefabs/StumpPrefab.h"

#include "../Prefabs/BonfirePrefab.h"
#include "../Prefabs/BombermanCharPrefab.h"

#include "../Materials/GrassMaterial.h"
#include "../Materials/SkyBoxMaterial.h"
#include "../Materials/Post/PostBloom.h"
#include "../Materials/Post/PostColorGrading.h"
#include "../../../Materials/Shadow/DiffuseMaterial_Shadow.h"

BombermanScene::BombermanScene() : GameScene(L"BombermanScene")
{
}

BombermanScene::~BombermanScene()
{
	BombManager::GetInstance()->DestroyInstance();
	BombermanGameSettings::GetInstance()->DestroyInstance();
}

void BombermanScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	auto& gameContext = GetGameContext();
	auto const gridSize = BombermanGameSettings::GetInstance()->GetGridSize();
	auto const blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();

	//Spawn the camera
	DirectX::XMFLOAT3 const cameraPos = { gridSize * blockSize / 1.98f
		,gridSize / 3.f * (BombermanGameSettings::GetInstance()->GetWallHeight() + 2) * blockSize,
		-blockSize * gridSize / 2.75f };
	DirectX::XMFLOAT3 const cameraRot = { 60,0,0 };
	CreateAndActivateCamera(cameraPos, cameraRot);

	gameContext.pShadowMapper->SetLight({ -100,150,-50 }, { 0.740129888f, -0.597205281f, 0.309117377f });

	CreatePlayers();
	CreateLevel();

	auto const bloomPp = new PostBloom();
	bloomPp->SetIntensity(0.35f);
	AddPostProcessingEffect(bloomPp);
	auto* pp = new PostColorGrading();
	pp->SetContribution(0.5f);
	pp->SetLUT(L"Resources/Textures/LUT.png");
	AddPostProcessingEffect(pp);

	//Sound
	FMOD::Sound* pSound;
	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/The 8-Bit Forest.mp3", FMOD_DEFAULT, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_NORMAL);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	SoundManager::GetInstance()->GetSystem()->playSound(pSound, nullptr, false, &m_pAmbientSoundChannel);
	m_pAmbientSoundChannel->setVolume(BombermanGameSettings::GetInstance()->GetMusicVolume());


	m_MenuActionID = gameContext.pInput->GetAvailableActionID();
	auto const input = InputAction(m_MenuActionID, InputTriggerState::Pressed, VK_ESCAPE);
	gameContext.pInput->AddInputAction(input);
}

void BombermanScene::Update()
{
	if (auto& gameContext = GetGameContext(); gameContext.pInput->IsActionTriggered(m_MenuActionID))
		LoadInGameMenu();
}

void BombermanScene::Draw()
{
}

void BombermanScene::CreateLevel()
{
	auto const gridSize = BombermanGameSettings::GetInstance()->GetGridSize();
	CreateGridFloor(gridSize);
	CreateWalls(gridSize);
	CreateNotDestructibleWalls(gridSize);
	CreateStumps(gridSize);
	CreateSkybox();
	CreateFloor();
	CreateCampSite();
	CreateVegetation();
}

void BombermanScene::CreateVegetation()
{
	auto const blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();
	auto const xStart = static_cast<float>(blockSize * (BombermanGameSettings::GetInstance()->GetGridSize() + 2.5));

	auto pObj = new GameObject();
	auto pModel = new ModelComponent(L"Resources/Meshes/Plane.ovm");
	auto pMat = new GrassMaterial(L"Resources/Effects/Grass.fx");
	auto matId = GetGameContext().pMaterialManager->AddMaterial(pMat);
	pModel->SetMaterial(matId);
	pObj->AddComponent(pModel);
	pMat->SetDiffuseTexture(L"Resources/Textures/Grass/Diffuse.png");
	pMat->SetHeightTexture(L"Resources/Textures/Grass/Height.png");
	pMat->SetWeightTexture(L"Resources/Textures/Grass/Weight.png");
	pMat->SetAddedColorTexture(L"Resources/Textures/Grass/AddColor.png");
	pMat->SetDirectionTexture(L"Resources/Textures/Grass/Direction.png");
	pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	pObj->GetTransform()->Translate(xStart - blockSize, 0, 0);
	pObj->GetTransform()->Scale(1.5f, 1.f, 2.f);

	AddChild(pObj);


	//Generate tree
	for (int i = 0; i < 20; i++)
	{
		pObj = new GameObject();

		pModel = new ModelComponent(L"Resources/Meshes/Tree.ovm");
		auto* pMaterial = new DiffuseMaterial_Shadow();
		matId = GetGameContext().pMaterialManager->AddMaterial(pMaterial);
		if (rand() % 2 == 0)
			pMaterial->SetDiffuseTexture(L"Resources/Textures/RedTree.png");
		else
			pMaterial->SetDiffuseTexture(L"Resources/Textures/GreenTree.png");

		pMaterial->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
		pModel->SetMaterial(matId);
		pObj->AddComponent(pModel);
		AddChild(pObj);
		pObj->GetTransform()->Scale(1.5f, 1.5f, 1.5f);
		pObj->GetTransform()->Rotate(0.f, randF(0, 360), 0.f);

		pObj->GetTransform()->Translate(xStart + rand() % 10, 0.f, randF(0.f, 80.f));
	}
}

void BombermanScene::CreateGridFloor(int const size)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);

	auto const blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();

	int const startX = blockSize / 2;
	int const startZ = blockSize / 2;

	auto pDiffuseMaterial1 = new DiffuseMaterial_Shadow();
	pDiffuseMaterial1->SetDiffuseTexture(L"./Resources/Textures/Soil1.jpg");
	auto const matID1 = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial1);

	auto pDiffuseMaterial2 = new DiffuseMaterial_Shadow();
	pDiffuseMaterial2->SetDiffuseTexture(L"./Resources/Textures/Soil2.jpg");
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
	pObj->GetTransform()->Translate(31, 0, 50);
}

void BombermanScene::CreateWalls(int const size)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);

	auto const blockSize = static_cast<float>(BombermanGameSettings::GetInstance()->GetBlockSize());

	float startX = -blockSize / 2.f;
	float startZ = -blockSize / 2.f;
	auto const halfSize = blockSize / 2.f;

	auto pDiffuseMaterial = new DiffuseMaterial_Shadow();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Log.png");
	auto const matId = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial);

	std::shared_ptr<physx::PxGeometry>geometry(new physx::PxBoxGeometry(blockSize, halfSize, halfSize));
	for (int z = 0; z < 2; ++z)
	{
		for (int x = 1; x < size + 1; ++x)
		{
			float const offsetX = static_cast<float>(x) * blockSize;
			auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Fence.ovm");
			pModelComponent->SetMaterial(matId);

			auto* pWall = new GameObject();
			pWall->AddComponent(pModelComponent);
			pWall->GetTransform()->Translate(startX + offsetX, 3 * blockSize / 2.f, startZ);

			auto const rb = new RigidBodyComponent();
			rb->SetKinematic(true);
			rb->SetCollisionGroup(CollisionGroupFlag::Group0);
			pWall->AddComponent(rb);

			auto* cc = new ColliderComponent(geometry, *bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1))));
			pWall->AddComponent(cc);
			AddChild(pWall);
			pWall->GetTransform()->Rotate(0, 90, 0);

		}
		startZ += (static_cast<float>(size) + 1.f) * blockSize;
	}

	startZ = blockSize / 2;
	for (int x = 0; x < 2; ++x)
	{
		{
			for (int z = 0; z < size; ++z)
			{
				float const offsetZ = z * blockSize;
				auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/Fence.ovm");
				pModelComponent->SetMaterial(matId);

				auto* pWall = new GameObject();
				pWall->AddComponent(pModelComponent);

				pWall->GetTransform()->Translate(startX, 3 * blockSize / 2.f, startZ + offsetZ);

				auto const rb = new RigidBodyComponent(true);
				rb->SetCollisionGroup(CollisionGroupFlag::Group0);
				pWall->AddComponent(rb);

				auto* cc = new ColliderComponent(geometry, *bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1))));

				pWall->AddComponent(cc);
				AddChild(pWall);
			}
		}
		startX += (size + 1) * blockSize;
	}

	for (int i = 0; i < 4; ++i)
	{
		float rotation = 0.f;
		switch (i)
		{
		case 0:
			rotation = -90.f;
			startX = -blockSize / 2.f;
			startZ = -blockSize / 2.f;
			break;
		case 1:
			startX = -blockSize / 2.f;
			startZ = blockSize / 2.f + (size)*blockSize;
			break;
		case 2:
			rotation = 180.f;
			startX = blockSize / 2.f + (size)*blockSize;
			startZ = -blockSize / 2.f;
			break;
		case 3:
			rotation = 90.f;

			startX = blockSize / 2.f + (size)*blockSize;
			startZ = blockSize / 2.f + (size)*blockSize;
			break;
		}

		auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/LogCorner.ovm");
		pModelComponent->SetMaterial(matId);
		auto* pWall = new GameObject();
		pWall->AddComponent(pModelComponent);

		pWall->GetTransform()->Translate(startX, 3 * blockSize / 2.f, startZ);

		auto const rb = new RigidBodyComponent(false);
		rb->SetKinematic(true);
		rb->SetCollisionGroup(CollisionGroupFlag::Group0);
		pWall->AddComponent(rb);

		auto* cc = new ColliderComponent(geometry, *bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1))));

		pWall->AddComponent(cc);
		AddChild(pWall);
		pWall->GetTransform()->Rotate(0.f, rotation, 0.f);
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

	auto const blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();

	int const startX = blockSize / 2;
	int const startZ = blockSize / 2;
	auto const halfSize = blockSize / 2.f;

	auto pDiffuseMaterial = new DiffuseMaterial_Shadow();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/Wall.png");
	auto const matId = GetGameContext().pMaterialManager->AddMaterial(pDiffuseMaterial);
	std::shared_ptr<physx::PxGeometry>geometry(new physx::PxBoxGeometry(static_cast<physx::PxReal>(blockSize), halfSize, halfSize));

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

			pWall->GetTransform()->Translate(static_cast<float>(startX + offsetX), blockSize * 1.f,
				static_cast<float>(startZ + offsetZ));

			auto rb = new RigidBodyComponent(true);
			rb->SetCollisionGroup(CollisionGroupFlag::Group0);
			pWall->AddComponent(rb);

			auto* cc = new ColliderComponent(geometry, *bouncyMaterial,
				physx::PxTransform(
					physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1))));

			pWall->AddComponent(cc);
			AddChild(pWall);
		}
	}
}

void BombermanScene::CreateStumps(int const size)
{
	if (size <= 5)
	{
		Logger::LogWarning(L"Size can't be less than 5");
		return;
	}

	auto const blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();

	int const startX = blockSize / 2;
	int const startZ = blockSize / 2;
	auto pDiffuseMaterial = new DiffuseMaterial_Shadow();
	pDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/TreeStump.png");
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
			if (!(static_cast<float>(rand() % 100) < BombermanGameSettings::GetInstance()->GetBreakableSpawnPercent() * 100))
				continue;

			auto* pWall = new StumpPrefab(matId);
			pWall->GetTransform()->Translate(static_cast<float>(startX + offsetX), 3 * blockSize / 2.f, static_cast<float>(startZ + offsetZ));
			AddChild(pWall);

		}
	}
}

void BombermanScene::CreateSkybox()
{
	auto& gameContext = GetGameContext();

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
	skyboxGameObject->GetTransform()->Translate(0, -250, 0);
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
	auto& pWindow = OverlordGame::GetGameSettings().Window;

	std::wstring const meshPath = L"./Resources/Meshes/Bomberman.ovm";

	if (BombermanGameSettings::GetInstance()->GetAmountOfPlayers() < 2)
	{
		Logger::LogError(L"BombermanScene::CreatePlayers(), the game has to have at least 2 players!", true);
	}

	auto const blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();
	float const startX = BombermanGameSettings::GetInstance()->GetGridSize() * blockSize - blockSize / 2.f;
	float const startY = blockSize * 1.5f;

	auto& schemeMap = BombermanGameSettings::GetInstance()->GetSchemeMap();
	auto drawPos = DirectX::XMFLOAT2(pWindow.Width / 7.5f, pWindow.Height * 1.f);

	for (int playerIndex = 0; playerIndex < BombermanGameSettings::GetInstance()->GetAmountOfPlayers(); playerIndex++)
	{
		auto const inputScheme = BombermanGameSettings::GetInstance()->GetPlayerInputScheme(playerIndex);
		auto& controls = schemeMap.at(inputScheme).first;

		BombermanCharPrefab* pCharacter{};

		if (static_cast<int>(inputScheme) >= static_cast<int>(InputScheme::Controller))
			pCharacter = new BombermanCharPrefab(meshPath,
				L"./Resources/Textures/Bomberman/Player" + std::to_wstring(
					playerIndex + 1) + L".png",
				controls, static_cast<GamepadIndex>(playerIndex), true);
		else
			pCharacter = new BombermanCharPrefab(meshPath,
				L"./Resources/Textures/Bomberman/Player" + std::to_wstring(
					playerIndex + 1) + L".png",
				controls, static_cast<GamepadIndex>(playerIndex), false);
		AddChild(pCharacter);

		if (playerIndex / 2)
			pCharacter->GetTransform()->Translate(startX, startY,
				playerIndex % 2 * (BombermanGameSettings::GetInstance()->GetGridSize() - 1) *
				blockSize + blockSize / 2.f);
		else
			pCharacter->GetTransform()->Translate(blockSize / 2.f, startY,
				playerIndex % 2 * (BombermanGameSettings::GetInstance()->GetGridSize() - 1) *
				blockSize + blockSize / 2.f);

		auto const UI = new BombermanUi(L"Resources/Textures/Bomberman/UIPlayer" + std::to_wstring(playerIndex + 1) + L".png", pCharacter, drawPos);
		UI->GetTransform()->Scale(0.25f, 0.25f, 0.25f);
		AddChild(UI);

		drawPos.x += pWindow.Width / 5.f;
	}
}

void BombermanScene::CreateCampSite()
{
	auto* const pBonfire = new BonfirePrefab({ -25.f,0.f,40.f });
	pBonfire->GetTransform()->Scale(2.f, 2.f, 2.f);
	AddChild(pBonfire);

	//Benches
	for (int i = 0; i < 2; i++)
	{
		auto* const pObj = new GameObject();

		auto* const pModelComp = new ModelComponent(L"Resources/Meshes/LogBench.ovm");
		auto* const pMat = new DiffuseMaterial_Shadow();
		auto const matId = GetGameContext().pMaterialManager->AddMaterial(pMat);
		pMat->SetDiffuseTexture(L"Resources/Textures/LogBench.png");
		pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
		pModelComp->SetMaterial(matId);
		pObj->AddComponent(pModelComp);
		AddChild(pObj);
		pObj->GetTransform()->Scale(0.3f, 0.3f, 0.3f);
		pObj->GetTransform()->Translate(-25.f, 0.f, 40.f + static_cast<float>(pow(-1, i)) * 15.f);
	}
}

void BombermanScene::LoadInGameMenu() const
{
	SceneManager::GetInstance()->SetActiveGameScene(L"In-game menu");
}

void BombermanScene::SceneActivated()
{
	if (m_pAmbientSoundChannel)
	{
		m_pAmbientSoundChannel->setPaused(false);
	}
}

void BombermanScene::SceneDeactivated()
{
	if (m_pAmbientSoundChannel)
	{
		m_pAmbientSoundChannel->setPaused(true);
	}
}
