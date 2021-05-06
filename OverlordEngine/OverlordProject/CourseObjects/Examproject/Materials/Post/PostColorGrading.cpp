#include "stdafx.h"
#include "PostColorGrading.h"
#include "RenderTarget.h"
#include "ContentManager.h"
#include "TextureData.h"

PostColorGrading::PostColorGrading():PostProcessingMaterial(L"Resources/Effects/Post/ColorGrading.fx")
                                     ,m_pContributionVariable(nullptr)
                                     ,m_pLookUpTextSRVariable(nullptr)
                                     ,m_pSceneMapVariable(nullptr)
                                     ,m_pLUTText(nullptr)
{
}

PostColorGrading::~PostColorGrading()
{
}

void PostColorGrading::LoadEffectVariables()
{
	m_pLookUpTextSRVariable = GetEffect()->GetVariableByName("gLUT")->AsShaderResource();
	if (!m_pLookUpTextSRVariable->IsValid())
	{
		Logger::LogWarning(L"PostColorGrading::LoadEffectVariables() > \'gLUT\' variable not found!");
		m_pLookUpTextSRVariable = nullptr;
	}
	m_pSceneMapVariable = GetEffect()->GetVariableByName("gRenderedText")->AsShaderResource();
	if (!m_pSceneMapVariable->IsValid())
	{
		Logger::LogWarning(L"PostColorGrading::LoadEffectVariables() > \'gRenderedText\' variable not found!");
		m_pSceneMapVariable = nullptr;
	}
	m_pContributionVariable = GetEffect()->GetVariableByName("gContribution")->AsScalar();
	if (!m_pContributionVariable->IsValid())
	{
		Logger::LogWarning(L"PostColorGrading::LoadEffectVariables() > \'gContribution\' variable not found!");
		m_pContributionVariable = nullptr;
	}
}

void PostColorGrading::UpdateEffectVariables(RenderTarget* rendertarget)
{
	if (m_pLUTText)
	{
		m_pLookUpTextSRVariable->SetResource(m_pLUTText->GetShaderResourceView());
	}
	m_pSceneMapVariable->SetResource(rendertarget->GetShaderResourceView());
	m_pContributionVariable->SetFloat(m_Contribution);
}

void PostColorGrading::SetContribution(float contribution)
{
	m_Contribution = contribution;
}

void PostColorGrading::SetLUT(const std::wstring& assetPath)
{
	m_pLUTText = ContentManager::Load<TextureData>(assetPath);
}
