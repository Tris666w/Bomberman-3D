#include "stdafx.h"
#include "Particle.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	if (!m_IsActive)
	{
		return;
	}

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();
	if (m_CurrentEnergy <= 0.f)
	{
		m_IsActive =false;
		return;
	}
	//Update position
	XMVECTOR movement = XMLoadFloat3(&m_EmitterSettings.Velocity);
	movement *= gameContext.pGameTime->GetElapsed();
	XMVECTOR position = XMLoadFloat3(&m_VertexInfo.Position);
	position += movement;
	XMStoreFloat3(&m_VertexInfo.Position, position);

	//Update color
	float const particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2.f;

	//Update size
	if (m_SizeGrow < 1.f)
		m_VertexInfo.Size = m_InitSize - m_SizeGrow * particleLifePercent;
	else
		m_VertexInfo.Size = m_InitSize + m_SizeGrow * particleLifePercent;

	
}

void Particle::Init(XMFLOAT3 initPosition)
{
	m_IsActive = true;

	//Init energy
	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy,m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = m_TotalEnergy;

	//Init position
	XMFLOAT3 randomDir = {1.f,0.f,0.f};
	XMVECTOR dirVector = XMLoadFloat3(&randomDir);
	XMVECTOR const pos = XMLoadFloat3(&initPosition);
	XMMATRIX const randomRotMatrix = XMMatrixRotationRollPitchYaw(randF(-XM_PI,XM_PI),randF(-XM_PI,XM_PI),randF(-XM_PI,XM_PI));
	
	dirVector =	XMVector3TransformNormal(dirVector,randomRotMatrix);
	float const distance = randF(m_EmitterSettings.MinEmitterRange,m_EmitterSettings.MaxEmitterRange);
	dirVector *= distance ;
	XMStoreFloat3(&m_VertexInfo.Position,dirVector + pos);

	//Init size
	m_VertexInfo.Size = randF(m_EmitterSettings.MinSize,m_EmitterSettings.MaxSize);
	m_InitSize = m_VertexInfo.Size;
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow,m_EmitterSettings.MaxSizeGrow);

	//Init rotation
	m_VertexInfo.Rotation = randF(-XM_PI,XM_PI);
}
