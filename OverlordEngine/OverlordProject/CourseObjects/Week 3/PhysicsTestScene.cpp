//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "PhysicsTestScene.h"

#include "ColliderComponent.h"
#include "GameObject.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "RigidBodyComponent.h"
#include "SpherePrefab.h"
#include "TransformComponent.h"
#

PhysicsTestScene::PhysicsTestScene(void) :
GameScene(L"PhysicsTestScene")
{}

void PhysicsTestScene::Initialize()
{
	Logger::LogInfo(L"Welcome, PhysicsTest!");
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	//GetPhysxProxy()->EnablePhysXFrameStepping(true);
	const auto& gameContext = GetGameContext();
	gameContext.pGameTime->ForceElapsedUpperbound(true);

	auto physx = PhysxManager::GetInstance()->GetPhysics();
	auto bouncyMaterial = physx->createMaterial(0.5, 0.5, 1.f);

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

	//Sphere 1
	auto pShere = new SpherePrefab();
	pShere->GetTransform()->Translate(0, 30, 0);

	auto pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);

	pRigidBody->SetCollisionIgnoreGroups(
		static_cast<CollisionGroupFlag>((UINT32)CollisionGroupFlag::Group1 | (UINT32)CollisionGroupFlag::Group2)
	);
	pShere->AddComponent(pRigidBody);

	std::shared_ptr<physx::PxGeometry> sphereGeom(new physx::PxSphereGeometry(1));
	ColliderComponent* pCC = new ColliderComponent(
		sphereGeom,
		*bouncyMaterial
	);
	pShere->AddComponent(pCC);
	AddChild(pShere);

	//Sphere 2 
	CreateSphere(bouncyMaterial, CollisionGroupFlag::Group1, 20);
	//Sphere 3
	CreateSphere(bouncyMaterial, CollisionGroupFlag::Group2, 10);
	
}

void PhysicsTestScene::CreateSphere(physx::PxMaterial* pMat, CollisionGroupFlag group, float y)
{
	auto pShere = new SpherePrefab();
	pShere->GetTransform()->Translate(0, y, 0);

	auto pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(group);

	pShere->AddComponent(pRigidBody);

	std::shared_ptr<physx::PxGeometry> sphereGeom(new physx::PxSphereGeometry(1));
	ColliderComponent* pCC = new ColliderComponent(
		sphereGeom,
		*pMat
	);
	pShere->AddComponent(pCC);
	AddChild(pShere);
}

void PhysicsTestScene::Update()
{
}

void PhysicsTestScene::Draw()
{
}
