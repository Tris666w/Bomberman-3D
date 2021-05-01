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
#include "..\..\Materials\SkyBoxMaterial.h"
#include "LinkGameObjectPosComponent.h"
#include "..\..\Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "ModelAnimator.h"

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
	
	GameObject* pCharacter = new BombermanCharPrefab(L"./Resources/Meshes/PeasantGirl.ovm",L"./Resources/Textures/PeasantGirl_Diffuse.png",
		m_BlockSize, 2.0f,5.0f,0.01f,50.f);
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

	CreateSkybox();
	CreateFloor(10);

	gameContext.pInput->AddInputAction(InputAction(10, InputTriggerState::Released, VK_SHIFT));
	
}

void BombermanScene::Update()
{
	if (GetGameContext().pInput->IsActionTriggered(10))
	{
		float const duration = 2.f;
		float const intensity = .05f;
		Logger::LogInfo(L"Shaking camera for " + std::to_wstring(duration) + L" with an intensity of "  + std::to_wstring(intensity));
		GetGameContext().pCamera->ShakeCamera(duration,intensity);
	}
	
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

void BombermanScene::CreateSkybox()
{
	const auto gameContext = GetGameContext();

	//Create game object and model
	auto const skyboxGameObject =new GameObject();
	auto* const pModelComponent = new ModelComponent(L"Resources/Meshes/Sphere.ovm");
	skyboxGameObject->AddComponent(pModelComponent);

	//Link geometry to camera (position) and scale the mesh
	auto* linkComp = new LinkGameObjectPosComponent(gameContext.pCamera->GetGameObject());
	skyboxGameObject->AddComponent(linkComp);
	skyboxGameObject->GetTransform()->Scale(10,20,10);

	//Create and assign material
	auto* pDiffuseMat = new SkyBoxMaterial();
	pDiffuseMat->SetCubeMapTexture(L"Resources/Textures/Skybox/Night.dds");
	auto const matID =  gameContext.pMaterialManager->AddMaterial(pDiffuseMat);
	pModelComponent->SetMaterial(matID);
	
	AddChild(skyboxGameObject);
}
