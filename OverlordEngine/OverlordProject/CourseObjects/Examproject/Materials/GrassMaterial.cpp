#include "stdafx.h"
#include "GrassMaterial.h"

#include "ContentManager.h"
#include "TextureData.h"

ID3DX11EffectScalarVariable* GrassMaterial::m_pElapsedTimeVariable = nullptr;
ID3DX11EffectVectorVariable* GrassMaterial::m_pLightDirectionVariable = nullptr;
ID3DX11EffectShaderResourceVariable* GrassMaterial::m_pDiffuseSRVariable = nullptr;
ID3DX11EffectShaderResourceVariable* GrassMaterial::m_pHeightMapSRVariable = nullptr;
ID3DX11EffectShaderResourceVariable* GrassMaterial::m_pDirectionMapSRVariable = nullptr;
ID3DX11EffectShaderResourceVariable* GrassMaterial::m_pWeightSRVariable = nullptr;
ID3DX11EffectShaderResourceVariable* GrassMaterial::m_pAddedColorSRVariable = nullptr;

GrassMaterial::GrassMaterial(const std::wstring& effectPath) :Material(effectPath, L"")
{
}

GrassMaterial::~GrassMaterial()
{
}

void GrassMaterial::LoadEffectVariables()
{
	m_pElapsedTimeVariable = GetEffect()->GetVariableByName("gElapsedSec")->AsScalar();
	if (!m_pElapsedTimeVariable->IsValid())
	{
		std::wcout << L"GrassMaterial::LoadEffectVariables(), m_pElapsedTimeVariable not valid\n";
	}
	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pLightDirectionVariable->IsValid())
	{
		std::wcout << L"GrassMaterial::LoadEffectVariables(), m_pLightDirectionVariable not valid\n";
	}
	m_pDiffuseSRVariable = GetEffect()->GetVariableByName("gGrassTexture")->AsShaderResource();
	if (!m_pDiffuseSRVariable->IsValid())
	{
		std::wcout << L"GrassMaterial::LoadEffectVariables(), m_pDiffuseSRVariable not valid\n";
	}
	m_pHeightMapSRVariable = GetEffect()->GetVariableByName("gHeightMap")->AsShaderResource();
	if (!m_pHeightMapSRVariable->IsValid())
	{
		std::wcout << L"GrassMaterial::LoadEffectVariables(), m_pHeightMapSRVariable not valid\n";
	}
	m_pDirectionMapSRVariable = GetEffect()->GetVariableByName("gDirectionTexture")->AsShaderResource();
	if (!m_pDirectionMapSRVariable->IsValid())
	{
		std::wcout << L"GrassMaterial::LoadEffectVariables(), m_pDirectionMapSRVariable not valid\n";
	}
	m_pAddedColorSRVariable = GetEffect()->GetVariableByName("gAddColorMap")->AsShaderResource();
	if (!m_pAddedColorSRVariable->IsValid())
	{
		std::wcout << L"GrassMaterial::LoadEffectVariables(), m_pAddedColorSRVariable not valid\n";
	}
	m_pWeightSRVariable = GetEffect()->GetVariableByName("gWeightMap")->AsShaderResource();
	if (!m_pWeightSRVariable->IsValid())
	{
		std::wcout << L"GrassMaterial::LoadEffectVariables(), m_pWeightSRVariable not valid\n";
	}
}

void GrassMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent*)
{
	if (m_CheckBools)
	{
		CheckBooleans();
	}
	m_pElapsedTimeVariable->SetFloat(gameContext.pGameTime->GetTotal());
	m_pLightDirectionVariable->SetFloatVector(&m_LightDirection.x);
	if (m_pDiffuseTexture)
		m_pDiffuseSRVariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	if (m_pHeightTexture)
		m_pHeightMapSRVariable->SetResource(m_pHeightTexture->GetShaderResourceView());

	if (m_pDirectionTexture)
		m_pDirectionMapSRVariable->SetResource(m_pDirectionTexture->GetShaderResourceView());

	if (m_pWeightTexture)
		m_pWeightSRVariable->SetResource(m_pWeightTexture->GetShaderResourceView());

	if (m_pAddedColorTexture)
		m_pAddedColorSRVariable->SetResource(m_pAddedColorTexture->GetShaderResourceView());
}

void GrassMaterial::CheckBooleans()
{
	m_CheckBools = false;
	if (!m_pDiffuseTexture)
	{
		auto useDiffuse = GetEffect()->GetVariableByName("gUseDiffuseTexture")->AsScalar();
		if (!useDiffuse->IsValid())
		{
			Logger::LogWarning(L"GrassMaterial::SetDiffuseTexture > \'gUseDiffuseTexture\' variable not found!");
		}
		useDiffuse->SetBool(true);
	}

	if (!m_pHeightTexture)
	{
		auto useHeight = GetEffect()->GetVariableByName("gUseHeightMap")->AsScalar();
		if (!useHeight->IsValid())
		{
			Logger::LogWarning(L"GrassMaterial::SetHeightTexture > \'gUseHeightMap\' variable not found!");
		}
		useHeight->SetBool(true);
	}

	if (!m_pDirectionTexture)
	{
		auto useDirection = GetEffect()->GetVariableByName("gUseDirectionTexture")->AsScalar();
		if (!useDirection->IsValid())
		{
			Logger::LogWarning(L"GrassMaterial::SetDirectionTexture > \'gUseDirectionTexture\' variable not found!");
		}
		useDirection->SetBool(true);
	}

	if (!m_pWeightTexture)
	{
		auto useWeight = GetEffect()->GetVariableByName("gUseWeightMap")->AsScalar();
		if (!useWeight->IsValid())
		{
			Logger::LogWarning(L"GrassMaterial::SetWeightTexture > \'gUseWeightMap\' variable not found!");
		}
		useWeight->SetBool(true);
	}

	if (!m_pAddedColorTexture)
	{
		auto useAddedColor = GetEffect()->GetVariableByName("gUseAddedColorTexture")->AsScalar();
		if (!useAddedColor->IsValid())
		{
			Logger::LogWarning(L"GrassMaterial::SetAddedColorTexture > \'gUseAddedColorTexture\' variable not found!");
		}
		useAddedColor->SetBool(true);
	}
}

void GrassMaterial::SetLightDirection(DirectX::XMFLOAT3 direction)
{
	m_LightDirection = direction;
}

void GrassMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_CheckBools = true;
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void GrassMaterial::SetHeightTexture(const std::wstring& assetFile)
{
	m_CheckBools = true;
	m_pHeightTexture = ContentManager::Load<TextureData>(assetFile);
}

void GrassMaterial::SetDirectionTexture(const std::wstring& assetFile)
{
	m_CheckBools = true;
	m_pDirectionTexture = ContentManager::Load<TextureData>(assetFile);
}

void GrassMaterial::SetWeightTexture(const std::wstring& assetFile)
{
	m_CheckBools = true;
	m_pWeightTexture = ContentManager::Load<TextureData>(assetFile);
}

void GrassMaterial::SetAddedColorTexture(const std::wstring& assetFile)
{
	m_CheckBools = true;
	m_pAddedColorTexture = ContentManager::Load<TextureData>(assetFile);
}
