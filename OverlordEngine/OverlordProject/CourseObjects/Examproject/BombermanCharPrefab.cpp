#include "stdafx.h"
#include "BombermanCharPrefab.h"

#include "BombermanGameSettings.h"
#include "PhysxManager.h"
#include "Components.h"
#include "BombManager.h"
#include"BombPrefab.h"
#include "GameScene.h"
#include "ModelAnimator.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"

BombermanCharPrefab::BombermanCharPrefab(const std::wstring& meshFilePath, const std::wstring& materialFilePath, float radius, float height,float stepOffset, float moveSpeed)
	:m_Radius(radius),
	m_Height(height),
	m_StepOffset(stepOffset),
	m_MoveSpeed(moveSpeed),
	m_pController(nullptr),

	//Running
	m_MaxRunVelocity(50.0f), 
	m_TerminalVelocity(20),
	m_Gravity(1.f), 
	m_RunAccelerationTime(0.3f), 
	m_JumpAccelerationTime(0.8f), 
	m_RunAcceleration(m_MaxRunVelocity/m_RunAccelerationTime), 
	m_JumpAcceleration(m_Gravity/m_JumpAccelerationTime), 
	m_RunVelocity(0), 
	m_JumpVelocity(0),
	m_Velocity(0,0,0),

	//File paths
	m_MeshFilePath(meshFilePath),
	m_MaterialFilePath(materialFilePath)

{
}

void BombermanCharPrefab::Initialize(const GameContext& gameContext)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto* pControllerComponent = new ControllerComponent(physx->createMaterial(0.f,0.f,0.f),m_Radius,m_Height,m_StepOffset,L"Bomberman"	);
	m_pController = pControllerComponent;
	AddComponent(pControllerComponent);

	//Add 1 bomb per player
	auto* pBomb = new BombPrefab();
	GetScene()->AddChild(pBomb);
	BombManager::GetInstance()->AddBomb(pBomb);


	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetDiffuseTexture(m_MaterialFilePath);
	auto const matID =  gameContext.pMaterialManager->AddMaterial(skinnedDiffuseMaterial);

	m_pModel = new ModelComponent(m_MeshFilePath,true);
	m_pModel->SetMaterial(matID);
	auto obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);
	m_pModel->GetTransform()->Scale(0.05f,0.05f,0.05f);
	m_pModel->GetTransform()->Translate(0,-m_Height/2 - m_Radius,0);


	//Input
	auto inputAction = InputAction(CharacterMovement::LEFT, InputTriggerState::Down, VK_LEFT);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::RIGHT, InputTriggerState::Down, VK_RIGHT);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::BACKWARD, InputTriggerState::Down, VK_DOWN);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::FORWARD, InputTriggerState::Down, VK_UP);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::DROP_BOMB, InputTriggerState::Pressed, VK_SPACE);
	gameContext.pInput->AddInputAction(inputAction);

}

void BombermanCharPrefab::PostInitialize(const GameContext& )
{
	m_pModel->GetAnimator()->SetAnimation(0);
	m_pModel->GetAnimator()->Play();
}

void BombermanCharPrefab::Update(const GameContext& gameContext)
{
	if (gameContext.pInput->IsActionTriggered(CharacterMovement::DROP_BOMB))
	{
		auto* pBomb = BombManager::GetInstance()->GetBomb();
		if (!pBomb)
			Logger::LogWarning(L"No bombs available?");
		else
		{
			pBomb->Activate(CalculateBombSpawnPos());
		}
		
	}

	using namespace DirectX;
	auto move = XMFLOAT2(0, 0);
	if (gameContext.pInput->IsActionTriggered(CharacterMovement::LEFT))
		move.x = -1.f;
	else if (gameContext.pInput->IsActionTriggered(CharacterMovement::RIGHT))
		move.x = 1.f;
	
	if (gameContext.pInput->IsActionTriggered(CharacterMovement::FORWARD))
		move.y = 1.f;
	else if (gameContext.pInput->IsActionTriggered(CharacterMovement::BACKWARD))
		move.y = -1.f;
	
	//If x or z movement is different from 0, calculate run velocity
	if (move.x != 0 || move.y != 0)
	{
		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
		if (m_RunVelocity>m_MaxRunVelocity)
			m_RunVelocity = m_MaxRunVelocity;

		m_Velocity.x = move.x * m_RunVelocity;
		m_Velocity.z = move.y * m_RunVelocity;
	}
	else
	{
		m_Velocity.x = 0;
		m_Velocity.z = 0;
	}


	//Apply gravity
	if (!m_pController->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_JumpVelocity -= m_Gravity * gameContext.pGameTime->GetElapsed();
		Clamp(m_JumpVelocity,m_TerminalVelocity,-m_TerminalVelocity);
		
	}
	else
		m_Velocity.y = 0;

	m_Velocity.y += m_JumpVelocity;
	
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);
	velocity *= gameContext.pGameTime->GetElapsed();

	XMFLOAT3 moveVelocity {};
	XMStoreFloat3(&moveVelocity,velocity);
	m_pController->Move(moveVelocity);
}

DirectX::XMFLOAT3 BombermanCharPrefab::CalculateBombSpawnPos() const
{
	DirectX::XMFLOAT3 spawnPos = {0,GetTransform()->GetWorldPosition().y - m_Height/3.f,0 };
	DirectX::XMFLOAT3 const worldPos = {GetTransform()->GetWorldPosition()};
	auto const blockSize = BombermanGameSettings::BlockSize;
	
	spawnPos.x = static_cast<float>((static_cast<int>(worldPos.x) / blockSize) * blockSize) + static_cast<float>(blockSize)/2.f;
	spawnPos.z = static_cast<float>((static_cast<int>(worldPos.z) / blockSize) * blockSize) + static_cast<float>(blockSize)/2.f;

	return spawnPos;
}
