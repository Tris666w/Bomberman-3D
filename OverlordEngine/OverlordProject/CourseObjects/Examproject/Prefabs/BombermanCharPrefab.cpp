#include "stdafx.h"
#include "BombermanCharPrefab.h"

#include "../BombermanGameSettings.h"
#include "../Scenes/BombermanScene.h"
#include "PhysxManager.h"
#include "Components.h"
#include "../BombManager.h"
#include"BombPrefab.h"
#include "ContentManager.h"
#include "GameScene.h"
#include "ModelAnimator.h"
#include "SceneManager.h"
#include "SoundManager.h"
#include "../../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../../../Materials/Shadow/DiffuseMaterial_Shadow.h"

BombermanCharPrefab::BombermanCharPrefab(const std::wstring& meshFilePath, const std::wstring& materialFilePath,
										 const std::vector<int>& controlKeyVect, GamepadIndex playerIndex,
										 bool useGamePad)
	:m_IsDead(false),
	m_Health(m_StartHealth),
	m_pController(nullptr),
	m_Radius(),
	m_Height(),
	m_StepOffset(),
	m_InputKeys(controlKeyVect),
	m_ActionIDs(),
	m_PlayerIndex(playerIndex),
	m_UseGamePad(useGamePad),
	//Running
	m_MaxRunVelocity(),
	m_TerminalVelocity(20),
	m_Gravity(1.f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.8f),
	m_RunAcceleration(0),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0),
	m_BombRange(m_BaseBombRange),
	//File paths
	m_MeshFilePath(meshFilePath),
	m_MaterialFilePath(materialFilePath),
	m_BombTimers()


{
}

void BombermanCharPrefab::Initialize(const GameContext& gameContext)
{
	m_Radius = BombermanGameSettings::GetInstance()->GetBlockSize() / 2.5f;
	m_Height = 5.0f;
	m_StepOffset = 0.01f;
	m_MaxRunVelocity = 25.f;
	m_RunAcceleration = m_MaxRunVelocity / m_RunAccelerationTime;

	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto* pControllerComponent = new ControllerComponent(physx->createMaterial(0.f, 0.f, 0.f), m_Radius, m_Height, m_StepOffset, L"Bomberman");
	SetTag(BombermanGameSettings::GetInstance()->GetPlayerTag());
	m_pController = pControllerComponent;
	AddComponent(pControllerComponent);

	//Add the max amount of bombs +1 
	for (int i = 0; i < m_AmountOfAvailableBombs + 1; ++i)
	{
		auto* pBomb = new BombPrefab();
		GetScene()->AddChild(pBomb);
		BombManager::GetInstance()->AddBomb(pBomb);
	}

	for (int i = 0; i < m_AmountOfAvailableBombs; ++i)
	{
		m_BombTimers.push_back(0);
	}
	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetDiffuseTexture(m_MaterialFilePath);
	auto const matID = gameContext.pMaterialManager->AddMaterial(skinnedDiffuseMaterial);

	m_pModel = new ModelComponent(m_MeshFilePath, true);
	m_pModel->SetMaterial(matID);
	AddComponent(m_pModel);

	//Input
#pragma region Keyboard
	if (!m_UseGamePad)
	{
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
#pragma endregion Keyboard
	}
#pragma region GamePad
	else
	{
		m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
		auto inputAction = InputAction(m_ActionIDs[0], InputTriggerState::Down, -1, 0, static_cast<WORD>(m_InputKeys[0]), m_PlayerIndex);
		gameContext.pInput->AddInputAction(inputAction);

		m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
		inputAction = InputAction(m_ActionIDs[1], InputTriggerState::Down, -1, 0, static_cast<WORD>(m_InputKeys[1]), m_PlayerIndex);
		gameContext.pInput->AddInputAction(inputAction);

		m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
		inputAction = InputAction(m_ActionIDs[2], InputTriggerState::Down, -1, 0, static_cast<WORD>(m_InputKeys[3]), m_PlayerIndex);
		gameContext.pInput->AddInputAction(inputAction);

		m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
		inputAction = InputAction(m_ActionIDs[3], InputTriggerState::Down, -1, 0, static_cast<WORD>(m_InputKeys[2]), m_PlayerIndex);
		gameContext.pInput->AddInputAction(inputAction);

		m_ActionIDs.push_back(gameContext.pInput->GetAvailableActionID());
		inputAction = InputAction(m_ActionIDs[4], InputTriggerState::Pressed, -1, 0, static_cast<WORD>(m_InputKeys[4]), m_PlayerIndex);
		gameContext.pInput->AddInputAction(inputAction);
#pragma endregion GamePad
	}

	//Initialize Sounds
	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	auto fmodResult = pFmodSystem->createStream("Resources/Sounds/Hit.wav", FMOD_DEFAULT, 0, &m_pHitSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	m_pHitSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);

	fmodResult = pFmodSystem->createStream("Resources/Sounds/Death.wav", FMOD_DEFAULT, 0, &m_pDeathSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	m_pDeathSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);

}

void BombermanCharPrefab::PostInitialize(const GameContext&)
{
	m_pModel->GetAnimator()->SetAnimation(L"Idle");
	m_pModel->GetAnimator()->Play();
}

void BombermanCharPrefab::Update(const GameContext& gameContext)
{
	if (m_IsDead)
		return;

	for (auto& timer : m_BombTimers)
	{
		if (m_AmountOfAvailableBombs < m_MaxAmountOfBombs)
		{
			timer += gameContext.pGameTime->GetElapsed();
			if (timer >= m_BombCooldown)
			{
				m_AmountOfAvailableBombs++;
				timer = 0.f;
			}
		}
	}

	if (gameContext.pInput->IsActionTriggered(m_ActionIDs[4]))
	{
		if (m_AmountOfAvailableBombs > 0)
		{
			auto* pBomb = BombManager::GetInstance()->GetBomb();
			if (!pBomb)
			{
				pBomb = new BombPrefab();
				GetScene()->AddChild(pBomb);
				BombManager::GetInstance()->AddBomb(pBomb);
			}
			else
			{
				pBomb->Activate(CalculateBombSpawnPos(),m_BombRange);
				--m_AmountOfAvailableBombs;
			}
		}
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
		if (AreEqual(m_Velocity.x, 0) && AreEqual(m_Velocity.z, 0))
			m_pModel->GetAnimator()->SetAnimation(L"Walk");
		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
		if (m_RunVelocity > m_MaxRunVelocity)
			m_RunVelocity = m_MaxRunVelocity;

		m_Velocity.x = move.x * m_RunVelocity;
		m_Velocity.z = move.y * m_RunVelocity;
	}
	else
	{
		if (!AreEqual(m_Velocity.x, 0) || !AreEqual(m_Velocity.z, 0))
			m_pModel->GetAnimator()->SetAnimation(L"Idle");

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

	if (moveVelocity.x > 0)
	{
		GetTransform()->Rotate(0, -90, 0);

	}
	else if (moveVelocity.x < 0)
	{
		GetTransform()->Rotate(0, 90, 0);
	}
	if (moveVelocity.z > 0)
	{
		GetTransform()->Rotate(0, 180, 0);

	}
	else if (moveVelocity.z < 0)
	{
		GetTransform()->Rotate(0, 0, 0);
	}
}

void BombermanCharPrefab::KillPlayer()
{
	m_IsDead = true;
	FMOD::Channel* pChannel;
	SoundManager::GetInstance()->GetSystem()->playSound(m_pDeathSound, nullptr, false, &pChannel);
	pChannel->setVolume(BombermanGameSettings::GetInstance()->GetSoundVolume());
	m_pModel->GetAnimator()->Pause();

	GetTransform()->Translate(10000, 10000, 10000);
	
	auto const scene = SceneManager::GetInstance()->GetActiveScene();
	reinterpret_cast<BombermanScene*>(scene)->CheckForGameEnd();
}

DirectX::XMFLOAT3 BombermanCharPrefab::CalculateBombSpawnPos() const
{
	DirectX::XMFLOAT3 spawnPos = { 0,GetTransform()->GetWorldPosition().y - m_Height / 3.f,0 };
	DirectX::XMFLOAT3 const worldPos = { GetTransform()->GetWorldPosition() };
	auto const blockSize = BombermanGameSettings::GetInstance()->GetBlockSize();

	spawnPos.x = static_cast<float>((static_cast<int>(worldPos.x) / blockSize) * blockSize) + static_cast<float>(blockSize) / 2.f;
	spawnPos.z = static_cast<float>((static_cast<int>(worldPos.z) / blockSize) * blockSize) + static_cast<float>(blockSize) / 2.f;

	return spawnPos;
}

void BombermanCharPrefab::Draw(const GameContext&)
{
}

void BombermanCharPrefab::DamagePlayer(int amount)
{
	m_Health -= amount;
	if (m_Health > 0)
	{
		FMOD::Channel* pChannel;
		SoundManager::GetInstance()->GetSystem()->playSound(m_pHitSound, nullptr, false, &pChannel);
		pChannel->setVolume(BombermanGameSettings::GetInstance()->GetSoundVolume());
	}
	else
	{
		KillPlayer();
	}
}

void BombermanCharPrefab::SetAmountOfBombs(int amount)
{
	m_AmountOfAvailableBombs = amount;
}

void BombermanCharPrefab::SetBombRange(int newRange)
{
	m_BombRange = newRange;
}

void BombermanCharPrefab::AddLife()
{
	++m_Health;
	Clamp(m_Health,m_MaxHealth,0);
}
