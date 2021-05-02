#include "stdafx.h"
#include "PostGrayscale.h"
#include "RenderTarget.h"

PostGrayscale::PostGrayscale()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Grayscale.fx", 1),
	m_pTextureMapVariabele(nullptr)
{
}

void PostGrayscale::LoadEffectVariables()
{
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"PostGrayScale::LoadEffectVariables() > \'gTexture\' variable not found!");
		m_pTextureMapVariabele = nullptr;
	}
}

void PostGrayscale::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	UNREFERENCED_PARAMETER(pRendertarget);
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}