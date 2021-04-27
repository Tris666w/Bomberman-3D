#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, int particleCount):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile))
{
	//TODO: See Lab9_2
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO: See Lab9_2
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2

	//BUFFER MAPPING CODE [PARTIAL :)]
	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//ParticleVertex* pBuffer = (ParticleVertex*) mappedResource.pData;
}

void ParticleEmitterComponent::Draw(const GameContext& )
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
}
