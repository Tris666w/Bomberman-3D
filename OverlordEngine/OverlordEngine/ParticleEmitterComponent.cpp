#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"
#include "GameObject.h"

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
	m_AssetFile(std::move(assetFile)),
	m_Particles(),
	m_IsActive(false)
{
	for (int i = 0; i < m_ParticleCount; ++i)
		m_Particles.push_back(new Particle(m_Settings));
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	for (auto& particle: m_Particles)
		delete particle;

	for (auto& pBurst: m_Bursts)
		delete pBurst;
	
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
	
}

void ParticleEmitterComponent::AddBurst(Burst* pBurst)
{
	if (std::find(m_Bursts.begin(),m_Bursts.end(),pBurst) != m_Bursts.end())
	{
		Logger::LogWarning(L"ParticleEmitterComponent::AddBurst, this burst is already in use");
	}
	m_Bursts.push_back(pBurst);
}

void ParticleEmitterComponent::RemoveBurst(Burst* pBurst)
{
	SafeDelete(pBurst);
	std::_Erase_remove(m_Bursts,pBurst);
}

void ParticleEmitterComponent::ResetBursts()
{
	for (auto& pBurst: m_Bursts)
		pBurst->Reset();
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");

	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

	//load variables
	m_pWvpVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pWvpVariable->IsValid())
	{
		std::wcout << L"m_pWvpVariable not valid\n";
	}
	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
	{
		std::wcout << L"m_pViewInverseVariable not valid\n";
	}
	m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();
	if (!m_pTextureVariable->IsValid())
	{
		std::wcout << L"m_pTextureVariable not valid\n";
	}
	
	if(!EffectHelper::BuildInputLayout(gameContext.pDevice,m_pDefaultTechnique,&m_pInputLayout,m_pInputLayoutSize))
		std::wcout << L"EffectHelper::BuildInputLayout --> did not succeed\n";
		
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	SafeRelease(m_pVertexBuffer);

	//Create dynamic buffer
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = m_ParticleCount * sizeof(ParticleVertex);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;

	HRESULT const result = gameContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pVertexBuffer);
	if (result != S_OK)
		Logger::LogHResult(result,L"ParticleEmitterComponent::CreateVertexBuffer",true);
	
}

void ParticleEmitterComponent::UpdateBursts(const GameContext& gameContext)
{
	for(auto& pBurst:m_Bursts)
	{
		if (pBurst->PassedCycles >= pBurst->Cycles && pBurst->Cycles != -1)
			continue;

			//Check if enough we can start the burst
			pBurst->TotalTimePast += gameContext.pGameTime->GetElapsed();
			pBurst->PassedIntervalTime += gameContext.pGameTime->GetElapsed();
		
			if (pBurst->TotalTimePast < pBurst->TriggerTime)
				continue;

			//Check if enough we can start the next interval
			if (pBurst->PassedIntervalTime < pBurst->IntervalTime)
				continue;
			
			++pBurst->PassedCycles;
			pBurst->PassedIntervalTime = 0.f;

			auto it = m_Particles.begin();
			int amountOfParticlesSpawned = 0;
			
			//Loop over all particles until we reach the end of the vector or we have spawned enough particles
			while (it != m_Particles.end() && amountOfParticlesSpawned < pBurst->Count)
			{
				//If the particle is inactive, spawn a new one and increase the counter
				if (!(*it)->IsActive())
				{
					(*it)->Init(GetTransform()->GetPosition());
					++amountOfParticlesSpawned;
				}
				++it;
			}
	}
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	m_LastParticleInit += gameContext.pGameTime->GetElapsed();
	
	m_ActiveParticles = 0;

	UpdateBursts(gameContext);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto* pBuffer = (ParticleVertex*) mappedResource.pData;

	for(auto& particle : m_Particles)
	{
		particle->Update(gameContext);
		if (particle->IsActive())
		{
			pBuffer[m_ActiveParticles] = particle->GetVertexInfo();
			++m_ActiveParticles;
		}
		else if (m_Settings.EmitRate == 0)
		{
			continue;
		}
		else if(m_LastParticleInit >= (1.f/m_Settings.EmitRate) && m_IsActive)
		{
			particle->Init(GetTransform()->GetPosition());
			pBuffer[m_ActiveParticles] = particle->GetVertexInfo();
			++m_ActiveParticles;
			m_LastParticleInit = 0;
		}
	}

	gameContext.pDeviceContext->Unmap(m_pVertexBuffer,0);
}

void ParticleEmitterComponent::Draw(const GameContext& )
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	m_pWvpVariable->SetMatrix(&gameContext.pCamera->GetViewProjection()._11);
	m_pViewInverseVariable->SetMatrix(&gameContext.pCamera->GetViewInverse()._11);
	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pDefaultTechnique->GetDesc(&techDesc);
	for (unsigned int i = 0; i < techDesc.Passes; ++i)
	{
		m_pDefaultTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
	
}
