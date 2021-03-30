#include "stdafx.h"
#include "CharacterScene.h"

#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "ControllerComponent.h"
#include "TransformComponent.h"
#include "Character.h"
#include "ModelComponent.h"
#include "../../Materials/DiffuseMaterial.h"

CharacterScene::CharacterScene():GameScene(L"CharacterScene")
{
}

void CharacterScene::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);

	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const baseMaterial = physx->createMaterial(0.2f, 0.2f, 0.1f);

	//ground (prefab)
	auto ground = new GameObject();
	ground->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry>geometry(new physx::PxPlaneGeometry());
	ground->AddComponent(
	new ColliderComponent(
		geometry,
		*baseMaterial, 
		physx::PxTransform(
			physx::PxQuat(DirectX::XM_PIDIV2,physx::PxVec3(0,0,1))))
	);
	AddChild(ground);
	
	GameObject* pCharacter = new Character();
	pCharacter->GetTransform()->Translate(0,50,0);
	AddChild(pCharacter);

	GameObject* level = new GameObject();
	auto* pModelComponent = new ModelComponent(L"Resources/Meshes/SimpleLevel.ovm");
	level->AddComponent(pModelComponent);
	auto* pDiffuseMat = new DiffuseMaterial();
	pDiffuseMat->SetDiffuseTexture(L"Resources/Textures/Chair_Dark.dds");
	GetGameContext().pMaterialManager->AddMaterial(pDiffuseMat, 0);
	
	pModelComponent->SetMaterial(0);
	AddChild(level);
}

void CharacterScene::Update()
{
}

void CharacterScene::Draw()
{
}
