#include "stdafx.h"
#include "PowerUps.h"

#include "ContentManager.h"
#include "ParticleEmitterComponent.h"
#include "SoundManager.h"
#include "TransformComponent.h"
#include "Prefabs/BombermanCharPrefab.h"

void RadiusPowerUp::ExecuteEffect(BombermanCharPrefab* pChar)
{
	if (GetIsEnabled())
	{
		SetIsEnabled(false);
		m_Activated = true;
		m_pAffectedChar = pChar;
		m_BaseBombRange = pChar->GetBaseBombRange();
		pChar->SetBombRange(m_BaseBombRange + m_RangeAdd);
		PlayEffectSound();
	}
}

void RadiusPowerUp::Initialize(const GameContext&)
{
	Init();

	auto& scale = GetTransform()->GetScale();
	float const avgScale = (scale.x + scale.y + scale.z) / 3.f;
	auto* const pBurst = new Burst(15,2,1.f,0);
	auto* pParticleComp = new ParticleEmitterComponent(L"Resources/Textures/Particles/PowerUpSmoke.png");
	pParticleComp->SetShape(EmitterShape::Sphere);
	pParticleComp->SetEmitRate(20);
	pParticleComp->SetVelocity(6.f);
	pParticleComp->SetMinSize(1.0f * avgScale);
	pParticleComp->SetMaxSize(2.0f * avgScale);
	pParticleComp->SetMinEnergy(0.75f * avgScale);
	pParticleComp->SetMaxEnergy(1.25f * avgScale);
	pParticleComp->SetMinSizeGrow(0.4f * avgScale);
	pParticleComp->SetMaxSizeGrow(1.5f * avgScale);
	pParticleComp->SetMinEmitterRange(0.5f * avgScale);
	pParticleComp->SetMaxEmitterRange(1.f * avgScale);
	pParticleComp->SetStartingColor(DirectX::XMFLOAT4(0.94f, 0.43f, 0.53f, 1.f));
	pParticleComp->SetEndingColor(DirectX::XMFLOAT4(0.94f, 0.43f, 0.53f, 0.f));
	pParticleComp->SetShape(EmitterShape::Cone);
	pParticleComp->AddBurst(pBurst);
	SetParticleEmitter(pParticleComp);

	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD::Sound* pSound;
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/PowerUps/PowerUp1.wav", FMOD_DEFAULT, 0, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	SetSound(pSound);
}

void RadiusPowerUp::Update(const GameContext& gameContext)
{
	if (m_Activated)
	{
		m_Timer += gameContext.pGameTime->GetElapsed();
		if (m_Timer > m_EffectTime)
		{
			m_Activated = false;
			m_pAffectedChar->SetBombRange(m_BaseBombRange);
			m_Timer = 0.f;
		}
	}
}

LifePowerUp::LifePowerUp():PowerUpBase()
{
}

void LifePowerUp::ExecuteEffect(BombermanCharPrefab* pChar)
{
	if (GetIsEnabled())
	{
		SetIsEnabled(false);
		pChar->AddLife();
		PlayEffectSound();
		
	}
}

void LifePowerUp::Initialize(const GameContext&)
{
	Init();

	auto& scale = GetTransform()->GetScale();
	float const avgScale = (scale.x + scale.y + scale.z) / 3.f;
	auto* const pBurst = new Burst(15,2,1.f,0);
	auto* pParticleComp = new ParticleEmitterComponent(L"Resources/Textures/Particles/PowerUpSmoke.png");
	pParticleComp->SetShape(EmitterShape::Sphere);
	pParticleComp->SetEmitRate(20);
	pParticleComp->SetVelocity(6.f);
	pParticleComp->SetMinSize(1.0f * avgScale);
	pParticleComp->SetMaxSize(2.0f * avgScale);
	pParticleComp->SetMinEnergy(0.75f * avgScale);
	pParticleComp->SetMaxEnergy(1.25f * avgScale);
	pParticleComp->SetMinSizeGrow(0.4f * avgScale);
	pParticleComp->SetMaxSizeGrow(1.5f * avgScale);
	pParticleComp->SetMinEmitterRange(0.5f * avgScale);
	pParticleComp->SetMaxEmitterRange(1.f * avgScale);
	pParticleComp->SetStartingColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	pParticleComp->SetEndingColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	pParticleComp->AddBurst(pBurst);
	SetParticleEmitter(pParticleComp);

	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD::Sound* pSound;
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/PowerUps/PowerUp2.wav", FMOD_DEFAULT, 0, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	SetSound(pSound);
}

void BombBeltPowerup::ExecuteEffect(BombermanCharPrefab* pChar)
{
	if (GetIsEnabled())
	{
		SetIsEnabled(false);
		pChar->SetAmountOfBombs(pChar->GetMaxAmountOfBombs());
		PlayEffectSound();
	}
}

void BombBeltPowerup::Initialize(const GameContext&)
{
	Init();

	auto& scale = GetTransform()->GetScale();
	float const avgScale = (scale.x + scale.y + scale.z) / 3.f;
	auto* const pBurst = new Burst(15,2,1.f,0);
	auto* pParticleComp = new ParticleEmitterComponent(L"Resources/Textures/Particles/PowerUpSmoke.png");
	pParticleComp->SetShape(EmitterShape::Sphere);
	pParticleComp->SetEmitRate(20);
	pParticleComp->SetVelocity(2.f);
	pParticleComp->SetMinSize(1.0f * avgScale);
	pParticleComp->SetMaxSize(2.0f * avgScale);
	pParticleComp->SetMinEnergy(0.5f * avgScale);
	pParticleComp->SetMaxEnergy(1.f * avgScale);
	pParticleComp->SetMinSizeGrow(0.4f * avgScale);
	pParticleComp->SetMaxSizeGrow(1.5f * avgScale);
	pParticleComp->SetMinEmitterRange(0.25f * avgScale);
	pParticleComp->SetMaxEmitterRange(0.5f * avgScale);
	pParticleComp->SetStartingColor(DirectX::XMFLOAT4(0.69f, 1.f, 0.73f, 1.f));
	pParticleComp->SetEndingColor(DirectX::XMFLOAT4(0.69f, 1.f, 0.73f, 0.f));
	pParticleComp->AddBurst(pBurst);
	SetParticleEmitter(pParticleComp);

	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	FMOD::Sound* pSound;
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/PowerUps/PowerUp3.wav", FMOD_DEFAULT, 0, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	SetSound(pSound);
}
