#include "stdafx.h"
#include "PostBloom.h"
#include "RenderTarget.h"

PostBloom::PostBloom()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Bloom.fx", 3),
	m_pTextureMapVariable(nullptr),
	m_pIntensityVariable(nullptr)
{
}

void PostBloom::SetIntensity(float intensity)
{
	m_Intensity = intensity;
}

void PostBloom::LoadEffectVariables()
{
	//Check if valid!
	m_pTextureMapVariable = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariable->IsValid())
	{
		Logger::LogWarning(L"PostGlow::LoadEffectVariables() > \'gTexture\' variable not found!");
		m_pTextureMapVariable = nullptr;
	}
	m_pIntensityVariable = GetEffect()->GetVariableByName("gIntensity")->AsScalar();
	if (!m_pIntensityVariable->IsValid())
	{
		Logger::LogWarning(L"PostGlow::LoadEffectVariables() > \'gIntensity\' variable not found!");
		m_pIntensityVariable = nullptr;
	}
}

void PostBloom::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	m_pTextureMapVariable->SetResource(pRendertarget->GetShaderResourceView());
	m_pIntensityVariable->SetFloat(m_Intensity);
}