#include "stdafx.h"
#include "PongScene.h"



#include "ColliderComponent.h"
#include "CubePrefab.h"
#include "GameObject.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "SpherePrefab.h"


PongScene::PongScene():GameScene(L"PongScene")
{
}

void PongScene::Initialize()
{
	const auto& gameContext = GetGameContext();


	//Camera
	auto camEmpty = new GameObject();
	auto cam = new CameraComponent();
	camEmpty->GetTransform()->Translate(0, 20, 0);
	camEmpty->GetTransform()->Rotate(90, 0, 0);
	camEmpty->AddComponent(cam);
	AddChild(camEmpty);
	SetActiveCamera(cam);

	//Input
	auto inputAction = InputAction(PongInput::RightPeddleUp, InputTriggerState::Down, VK_UP);
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PongInput::RightPeddleDown, InputTriggerState::Down, VK_DOWN);
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PongInput::LeftPeddleUp, InputTriggerState::Down, 'Y');
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PongInput::LeftPeddleDown, InputTriggerState::Down, 'H');
	gameContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(PongInput::LaunchBall, InputTriggerState::Down, VK_SPACE);
	gameContext.pInput->AddInputAction(inputAction);


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
	
	//Left paddle
	physx::PxVec3 const cubeDimensions = { 1,5,3 };
	m_pLeftPaddle = new CubePrefab(cubeDimensions.x,cubeDimensions.y,cubeDimensions.z);
	m_pLeftPaddle->GetTransform()->Translate(-12, 0, 0);

	auto* pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
	pRigidBody->SetKinematic(true);
	m_pLeftPaddle->AddComponent(pRigidBody);
	
	std::shared_ptr<physx::PxGeometry> cubeGeometry(new physx::PxBoxGeometry(cubeDimensions / 2.f));
	ColliderComponent* pColliderComponent = new ColliderComponent(cubeGeometry, *bouncyMaterial);
	m_pLeftPaddle->AddComponent(pColliderComponent);
	AddChild(m_pLeftPaddle);

	//Right paddle
	m_pRightPaddle = new CubePrefab(cubeDimensions.x,cubeDimensions.y,cubeDimensions.z);
	m_pRightPaddle->GetTransform()->Translate(12, 0, 0);

	pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
	pRigidBody->SetKinematic(true);
	m_pRightPaddle->AddComponent(pRigidBody);
	
	pColliderComponent = new ColliderComponent(cubeGeometry, *bouncyMaterial);
	m_pRightPaddle->AddComponent(pColliderComponent);
	AddChild(m_pRightPaddle);

	//Walls
	physx::PxVec3 const wallDimensions = { 40,5,2 };
	std::shared_ptr<physx::PxGeometry> wallGeometry(new physx::PxBoxGeometry(wallDimensions / 2.f));
	for (int i = 0; i<2;++i)
	{

		GameObject* pWall = new CubePrefab(wallDimensions.x, wallDimensions.y, wallDimensions.z, { 0.f,0.f,0.f,1.f });;
		pWall->GetTransform()->Translate(0, 0, (i*16.f)+-8.f);

		pRigidBody = new RigidBodyComponent(true);
		pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
		pWall->AddComponent(pRigidBody);

		
		pColliderComponent = new ColliderComponent(wallGeometry, *bouncyMaterial);
		pWall->AddComponent(pColliderComponent);
		AddChild(pWall);
	}

	//Sphere
	m_pSphere = new SpherePrefab();
	m_pSphere->GetTransform()->Translate(0, 0.5, 0);

	pRigidBody = new RigidBodyComponent();
	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
	m_pSphere->AddComponent(pRigidBody);

	std::shared_ptr<physx::PxGeometry> sphereGeom(new physx::PxSphereGeometry(1));
	pColliderComponent = new ColliderComponent(
		sphereGeom,
		*bouncyMaterial
	);
	m_pSphere->AddComponent(pColliderComponent);
	AddChild(m_pSphere);


	//TriggerZones
	physx::PxVec3 const triggerDimensions = { 5,2,13 };
	std::shared_ptr<physx::PxGeometry> triggerGeometry(new physx::PxBoxGeometry(triggerDimensions / 2.f));
	for (int i = 0; i<2;++i)
	{

		GameObject* pWall = new CubePrefab(triggerDimensions.x, triggerDimensions.y, triggerDimensions.z,{1.f,1.f,1.f,0.2f});;
		pWall->GetTransform()->Translate(-15.f + (i*30.f), 0, 0);

		pRigidBody = new RigidBodyComponent(true);
		pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);
		pWall->AddComponent(pRigidBody);

		
		pColliderComponent = new ColliderComponent(triggerGeometry, *bouncyMaterial);
		pColliderComponent->EnableTrigger(true);

		pWall->SetOnTriggerCallBack([this](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
			{
				if (action == GameObject::TriggerAction::LEAVE && other == m_pSphere)
				{
					m_CanResetSphere = true;
				}
				UNREFERENCED_PARAMETER(trigger);
			});
		
		pWall->AddComponent(pColliderComponent);
		AddChild(pWall);
	}
}

void PongScene::Update()
{
	const auto& gameContext = GetGameContext();

	if (m_CanResetSphere)
		ResetSphere();
	 
	
	//Right paddle
	auto rightPeddlePos = m_pRightPaddle->GetTransform()->GetWorldPosition();

	if (gameContext.pInput->IsActionTriggered(PongInput::RightPeddleUp))
	{
		rightPeddlePos.z += m_PeddleSpeed * gameContext.pGameTime->GetElapsed();
	}else if (gameContext.pInput->IsActionTriggered(PongInput::RightPeddleDown))
	{
		rightPeddlePos.z -= m_PeddleSpeed * gameContext.pGameTime->GetElapsed();
	}

	Clamp<float>(rightPeddlePos.z, 5, -5);
	m_pRightPaddle->GetTransform()->Translate(rightPeddlePos);

	//Left paddle
	auto leftPeddlePos = m_pLeftPaddle->GetTransform()->GetWorldPosition();

	if (gameContext.pInput->IsActionTriggered(PongInput::LeftPeddleUp))
	{
		leftPeddlePos.z += m_PeddleSpeed * gameContext.pGameTime->GetElapsed();
	}else if (gameContext.pInput->IsActionTriggered(PongInput::LeftPeddleDown))
	{
		leftPeddlePos.z -= m_PeddleSpeed * gameContext.pGameTime->GetElapsed();
	}

	Clamp<float>(leftPeddlePos.z, 5, -5);
	m_pLeftPaddle->GetTransform()->Translate(leftPeddlePos);

	//LaunchBall
	if (gameContext.pInput->IsActionTriggered(PongInput::LaunchBall))
	{
		float forceAmount = 10;
		m_pSphere->GetComponent<RigidBodyComponent>()->SetDensity(0.1f);
		m_pSphere->GetComponent<RigidBodyComponent>()->AddForce(forceAmount * ToPxVec3({ 5,0,5 }));
		
	}
}

void PongScene::Draw()
{
}

void PongScene::ResetSphere()
{
	m_pSphere->GetTransform()->Translate(0, 0.5, 0);
	m_pSphere->GetComponent<RigidBodyComponent>()->ClearForce();
	m_pSphere->GetComponent<RigidBodyComponent>()->ClearTorque();
	
	m_CanResetSphere = false;
}
