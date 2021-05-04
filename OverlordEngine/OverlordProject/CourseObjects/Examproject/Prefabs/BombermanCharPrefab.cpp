#include "stdafx.h"
#include "BombermanCharPrefab.h"

#include "../BombermanGameSettings.h"
#include "PhysxManager.h"
#include "Components.h"
#include "../BombManager.h"
#include"BombPrefab.h"
#include "GameScene.h"
#include "ModelAnimator.h"
#include "../../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../Components/DestroyableWallComponent.h"

BombermanCharPrefab::BombermanCharPrefab(const std::wstring& meshFilePath, const std::wstring& materialFilePath, float radius, float height, float stepOffset, float moveSpeed, const std::vector<int>& controlKeyVect, GamepadIndex playerIndex)
	:m_IsDead(false),
	m_pController(nullptr),
	m_Radius(radius),
	m_Height(height),
	m_StepOffset(stepOffset),
	m_InputKeys(controlKeyVect),
	m_ActionIDs(),
	m_PlayerIndex(playerIndex),

	//Running
	m_MaxRunVelocity(moveSpeed),
	m_TerminalVelocity(20),
	m_Gravity(1.f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.8f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0),

	//File paths
	m_MeshFilePath(meshFilePath),
	m_MaterialFilePath(materialFilePath)

{
}

void BombermanCharPrefab::Initialize(const GameContext& gameContext)
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto* pControllerComponent = new ControllerComponent(physx->createMaterial(0.f, 0.f, 0.f), m_Radius, m_Height, m_StepOffset, L"Bomberman");
	SetTag(BombermanGameSettings::player_tag);
	m_pController = pControllerComponent;
	AddComponent(pControllerComponent);

	//Add 1 bomb per player
	auto* pBomb = new BombPrefab();
	GetScene()->AddChild(pBomb);
	BombManager::GetInstance()->AddBomb(pBomb);


	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetDiffuseTexture(m_MaterialFilePath);
	auto const matID = gameContext.pMaterialManager->AddMaterial(skinnedDiffuseMaterial);

	m_pModel = new ModelComponent(m_MeshFilePath, true);
	m_pModel->SetMaterial(matID);
	auto obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);
	m_pModel->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	m_pModel->GetTransform()->Translate(0, -m_Height / 2 - m_Radius, 0);


	//Input
	m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
	auto inputAction = InputAction(m_ActionIDs[0], InputTriggerState::Down, m_InputKeys[0], 0, 0, m_PlayerIndex);
	gameContext.pInput->AddInputAction(inputAction);

	m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
	inputAction = InputAction(m_ActionIDs[1], InputTriggerState::Down, m_InputKeys[1], 0, 0, m_PlayerIndex);
	gameContext.pInput->AddInputAction(inputAction);

	m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
	inputAction = InputAction(m_ActionIDs[2], InputTriggerState::Down, m_InputKeys[3], 0, 0, m_PlayerIndex);
	gameContext.pInput->AddInputAction(inputAction);

	m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
	inputAction = InputAction(m_ActionIDs[3], InputTriggerState::Down, m_InputKeys[2], 0, 0, m_PlayerIndex);
	gameContext.pInput->AddInputAction(inputAction);

	m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
	inputAction = InputAction(m_ActionIDs[4], InputTriggerState::Pressed, m_InputKeys[4], 0, 0, m_PlayerIndex);
	gameContext.pInput->AddInputAction(inputAction);

}

void BombermanCharPrefab::PostInitialize(const GameContext&)
{
	m_pModel->GetAnimator()->SetAnimation(0);
	m_pModel->GetAnimator()->Play();
}

void BombermanCharPrefab::Update(const GameContext& gameContext)
{
	if (m_IsDead)
		return;

	if (m_CanSpawnBomb)
	{
		if (gameContext.pInput->IsActionTriggered(m_ActionIDs[4]))
		{
			auto* pBomb = BombManager::GetInstance()->GetBomb();
			if (!pBomb)
				Logger::LogWarning(L"No bombs available?");
			else
			{
				pBomb->Activate(CalculateBombSpawnPos());
				m_CanSpawnBomb = false;
				m_BombTimer = 0.f;
			}
		}
	}
	else
	{
		m_BombTimer += gameContext.pGameTime->GetElapsed();
		if (m_BombTimer >= m_BombCooldown)
			m_CanSpawnBomb = true;
	}

	using namespace DirectX;
	auto move = XMFLOAT2(0, 0);
	if (gameContext.pInput->IsActionTriggered(m_ActionIDs[0]))
		move.x = -1.f;
	else if (gameContext.pInput->IsActionTriggered(m_ActionIDs[1]))
		move.x = 1.f;

	if (gameContext.pInput->IsActionTriggered(m_ActionIDs[3]))
		move.y = 1.f;
	else if (gameContext.pInput->IsActionTriggered(m_ActionIDs[2]))
		move.y = -1.f;

	//If x or z movement is different from 0, calculate run velocity
	if (move.x != 0 || move.y != 0)
	{
		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
		if (m_RunVelocity > m_MaxRunVelocity)
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
		Clamp(m_JumpVelocity, m_TerminalVelocity, -m_TerminalVelocity);

	}
	else
		m_Velocity.y = 0;

	m_Velocity.y += m_JumpVelocity;

	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);
	velocity *= gameContext.pGameTime->GetElapsed();

	XMFLOAT3 moveVelocity{};
	XMStoreFloat3(&moveVelocity, velocity);
	m_pController->Move(moveVelocity);
}

void BombermanCharPrefab::KillPlayer()
{
	m_IsDead = true;
	//TODO setDeath animation + transform the player out of the game
}

DirectX::XMFLOAT3 BombermanCharPrefab::CalculateBombSpawnPos() const
{
	DirectX::XMFLOAT3 spawnPos = { 0,GetTransform()->GetWorldPosition().y - m_Height / 3.f,0 };
	DirectX::XMFLOAT3 const worldPos = { GetTransform()->GetWorldPosition() };
	auto const blockSize = BombermanGameSettings::block_size;

	spawnPos.x = static_cast<float>((static_cast<int>(worldPos.x) / blockSize) * blockSize) + static_cast<float>(blockSize) / 2.f;
	spawnPos.z = static_cast<float>((static_cast<int>(worldPos.z) / blockSize) * blockSize) + static_cast<float>(blockSize) / 2.f;

	return spawnPos;
}
