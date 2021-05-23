#include "stdafx.h"
#include "Explosion.h"
#include "ParticleEmitterComponent.h"

Explosion::Explosion()
{
}

Explosion::~Explosion()
{
}

bool Explosion::GetIsEnabled() const
{
	return m_IsEnabled;
}

void Explosion::SetIsEnabled(bool enabled)
{
	m_IsEnabled = enabled;
	m_pParticleEmitter->SetBurstActive(enabled);
	m_pParticleEmitter->ResetBursts();

}

void Explosion::Initialize(const GameContext&)
{
	//Explosion emitter
	auto const pBurst = new Burst(15, 1, 0, 0);
	m_pParticleEmitter = new ParticleEmitterComponent(L"Resources/Textures/Particles/smoke_04.png", 15);
	m_pParticleEmitter->SetStartingColor(DirectX::XMFLOAT4(1.f, .647f, 0.f, 1.f));
	m_pParticleEmitter->SetEndingColor(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	m_pParticleEmitter->SetShape(EmitterShape::Sphere);
	m_pParticleEmitter->SetVelocity(4.f);
	m_pParticleEmitter->SetMinSize(0.4f);
	m_pParticleEmitter->SetMaxSize(1.5f);
	m_pParticleEmitter->SetMinEnergy(1.f);
	m_pParticleEmitter->SetMaxEnergy(1.5f);
	m_pParticleEmitter->SetMinSizeGrow(1.f);
	m_pParticleEmitter->SetMaxSizeGrow(1.5f);
	m_pParticleEmitter->SetMinEmitterRange(.2f);
	m_pParticleEmitter->SetMaxEmitterRange(1.0f);
	m_pParticleEmitter->SetEmitRate(0);
	m_pParticleEmitter->AddBurst(pBurst);

	AddComponent(m_pParticleEmitter);
}

void Explosion::Draw(const GameContext&)
{
}

void Explosion::Update(const GameContext& gameContext)
{
	if (m_IsEnabled)
	{
		m_Timer += gameContext.pGameTime->GetElapsed();
		if (m_Timer > m_ExplodeTime)
		{
			m_Timer = 0.f;
			SetIsEnabled(false);
		}
	}
}
