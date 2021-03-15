#include "stdafx.h"
#include "UberScene.h"
#include "PhysxManager.h"
#include "Components.h"
#include "GameObject.h"
#include "../../Materials/DiffuseMaterial.h"
#include "../../Materials/UberMaterial.h"

UberScene::UberScene() :GameScene(L"UberScene")
{

}

void UberScene::Initialize()
{
		const auto gameContext = GetGameContext();

	//GROUND PLANE
	//************
	auto physX = PhysxManager::GetInstance()->GetPhysics();

	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	//TeaPot OBJECT
	//************
	m_pTeaPot = new GameObject();

	auto* pModelComponent = new ModelComponent(L"Resources/Meshes/Teapot.ovm");
	m_pTeaPot->AddComponent(pModelComponent);

	m_pMaterial = new UberMaterial();
	m_pMaterial->EnableDiffuseTexture(true);
	m_pMaterial->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");

	m_pMaterial->EnableSpecularBlinn(true);
	m_pMaterial->EnableSpecularPhong(true);
	
	m_pMaterial->EnableEnvironmentMapping(true);
	m_pMaterial->SetEnvironmentCube(L"Resources/Textures/Sunol_Cubemap.dds");
	m_pMaterial->SetReflectionStrength(0.5f);
	
	gameContext.pMaterialManager->AddMaterial(m_pMaterial, 0);
	
	pModelComponent->SetMaterial(0);
	AddChild(m_pTeaPot);
}

void UberScene::Update()
{
}

void UberScene::Draw()
{
}
