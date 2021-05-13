//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	for (auto& inputLayout : m_pInputLayouts)
			SafeRelease(inputLayout);

}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		//Init effect
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ShadowMapGenerator.fx");

		//Init techniques
		m_pShadowTechs[0] = m_pShadowEffect->GetTechniqueByIndex(0);
		m_pShadowTechs[1] = m_pShadowEffect->GetTechniqueByIndex(1);
		
		//Init shader variables
		m_pLightVPMatrixVariable = m_pShadowEffect->GetVariableByName("gLightViewProj")->AsMatrix();
		if (!m_pLightVPMatrixVariable->IsValid())
		{
			std::wcout << L"m_pLightVPMatrixVariable not valid\n";
		}
		m_pBoneTransforms = m_pShadowEffect->GetVariableByName("gBones")->AsMatrix();
		if (!m_pBoneTransforms->IsValid())
		{
			std::wcout << L"m_pBoneTransforms not valid\n";
		}
		m_pWorldMatrixVariable = m_pShadowEffect->GetVariableByName("gWorld")->AsMatrix();
		if (!m_pWorldMatrixVariable->IsValid())
		{
			std::wcout << L"m_pWorldMatrixVariable not valid\n";
		}
	
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[static_cast<int>(ShadowGenType::Static)], &m_pInputLayouts[static_cast<int>(ShadowGenType::Static)], 
			m_InputLayoutDescriptions[static_cast<int>(ShadowGenType::Static)],
m_InputLayoutSizes[static_cast<int>(ShadowGenType::Static)], m_InputLayoutIds[static_cast<int>(ShadowGenType::Static)]);
		
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[static_cast<int>(ShadowGenType::Skinned)], &m_pInputLayouts[static_cast<int>(ShadowGenType::Skinned)], 
			m_InputLayoutDescriptions[static_cast<int>(ShadowGenType::Skinned)],
m_InputLayoutSizes[static_cast<int>(ShadowGenType::Skinned)], m_InputLayoutIds[static_cast<int>(ShadowGenType::Skinned)]);
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	m_pLightVPMatrixVariable->SetMatrix(reinterpret_cast<float*>(&lightVP));
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	m_pBoneTransforms->SetMatrixArray(pData,0,count);
}
