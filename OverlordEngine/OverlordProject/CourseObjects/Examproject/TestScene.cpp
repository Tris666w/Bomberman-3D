#include "stdafx.h"
#include "TestScene.h"

#include "PowerUps.h"
#include "PhysxProxy.h"
#include "BombManager.h"
#include "BombermanUI.h"
#include "PhysxManager.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "BombermanGameSettings.h"
#include "OverlordGame.h"
#include "Prefabs/BombermanCharPrefab.h"

TestScene::TestScene() :GameScene(L"TestScene")
{

}

TestScene::~TestScene()
{
	BombManager::GetInstance()->DestroyInstance();
	BombermanGameSettings::GetInstance()->DestroyInstance();
}

void TestScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	auto& pWindow = OverlordGame::GetGameSettings().Window;

	m_pRadiusPU = new RadiusPowerUp();
	auto& schemeMap = BombermanGameSettings::GetInstance()->GetSchemeMap();
	auto const inputScheme = BombermanGameSettings::GetInstance()->GetPlayerInputScheme(0);
	auto& controls = schemeMap.at(inputScheme).first;
	std::wstring const meshPath = L"./Resources/Meshes/Bomberman.ovm";

	BombermanCharPrefab* pCharacter{};

	pCharacter = new BombermanCharPrefab(meshPath,
		L"./Resources/Textures/Bomberman/Player" + std::to_wstring(
			0 + 1) + L".png",
		controls, static_cast<GamepadIndex>(0), false);;

	AddChild(pCharacter);
	pCharacter->GetTransform()->Translate(0, 0, -10.f);

	auto const UI = new BombermanUi(L"Resources/Textures/Bomberman/UIPlayer" + std::to_wstring(1) + L".png", pCharacter, { pWindow.Width / 2.f,pWindow.Height / 5.f });
	UI->GetTransform()->Scale(0.25f, 0.25f, 0.25f);
	AddChild(UI);
	AddChild(m_pRadiusPU);

	m_pLifePU = new LifePowerUp();
	AddChild(m_pLifePU);
	m_pLifePU->GetTransform()->Translate(20.f, 0.f, 0.f);

	m_pBombBeltPU = new BombBeltPowerup();
	AddChild(m_pBombBeltPU);
	m_pBombBeltPU->GetTransform()->Translate(40.f, 0.f, 0.f);


	//****************
	//GROUND PLANE
	//****************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	m_ID = GetGameContext().pInput->GetAvailableActionID();
	auto input = InputAction(m_ID,InputTriggerState::Pressed,'R');
	GetGameContext().pInput->AddInputAction(input);
}



void TestScene::Update()
{
	if (GetGameContext().pInput->IsActionTriggered(m_ID))
	{
		m_pBombBeltPU->SetIsEnabled(true);
		m_pLifePU->SetIsEnabled(true);
		m_pRadiusPU->SetIsEnabled(true);
		
	}
}

void TestScene::Draw()
{
}
