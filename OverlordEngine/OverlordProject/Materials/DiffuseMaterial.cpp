#include "stdafx.h"
#include "DiffuseMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "utils.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial::m_pDiffuseSRVvariable = nullptr;

DiffuseMaterial::DiffuseMaterial(): Material(L"Resources/Effects/PosNormTex3d.fx")
	,m_pDiffuseTexture(nullptr)
{
}

DiffuseMaterial::~DiffuseMaterial()
{
}

void DiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	utils::SafeDelete(m_pDiffuseTexture);
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseSRVvariable->IsValid())
	{
		std::wcout << L"m_pDiffuseSRVvariable not valid\n";
	}
}

void DiffuseMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_pDiffuseSRVvariable->IsValid())
	{
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	}
	UNREFERENCED_PARAMETER(pModelComponent);
	UNREFERENCED_PARAMETER(gameContext);
}
