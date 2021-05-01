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
	m_InitSize(0),
	m_MoveDirection()
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
	XMVECTOR movement = XMLoadFloat3(&m_MoveDirection);
	movement *= gameContext.pGameTime->GetElapsed() * m_EmitterSettings.Velocity;
	XMVECTOR position = XMLoadFloat3(&m_VertexInfo.Position);
	position += movement;
	XMStoreFloat3(&m_VertexInfo.Position, position);

	//Update color
	float const particleLifePercent = m_CurrentEnergy / m_TotalEnergy;

	auto const newColor = XMVectorLerp(XMLoadFloat4(&m_EmitterSettings.EndingColor), XMLoadFloat4(&m_EmitterSettings.StartColor), particleLifePercent);
	XMStoreFloat4(&m_VertexInfo.Color,newColor);
	
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

	//Init position and direction
	m_MoveDirection = {1.f,0.f,0.f};
	XMVECTOR dirVector = XMLoadFloat3(&m_MoveDirection);
	XMVECTOR const pos = XMLoadFloat3(&initPosition);
	XMMATRIX const randomRotMatrix = XMMatrixRotationRollPitchYaw(randF(-XM_PI,XM_PI),randF(-XM_PI,XM_PI),randF(-XM_PI,XM_PI));
	
	dirVector =	XMVector3TransformNormal(dirVector,randomRotMatrix);
	float const distance = randF(m_EmitterSettings.MinEmitterRange,m_EmitterSettings.MaxEmitterRange);
	dirVector *= distance ;
	
	XMStoreFloat3(&m_VertexInfo.Position,dirVector + pos);
	
	m_VertexInfo.Color = m_EmitterSettings.StartColor;
	
	switch (m_EmitterSettings.m_Shape)
	{
		case EmitterShape::Cone:
			m_MoveDirection = m_EmitterSettings.MoveDirection;
			break;
		case EmitterShape::Sphere:
			XMStoreFloat3(&m_MoveDirection,dirVector);
			break;
	}

	//Init size
	m_VertexInfo.Size = randF(m_EmitterSettings.MinSize,m_EmitterSettings.MaxSize);
	m_InitSize = m_VertexInfo.Size;
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow,m_EmitterSettings.MaxSizeGrow);

	//Init rotation
	m_VertexInfo.Rotation = randF(-XM_PI,XM_PI);

	//Init color
}
