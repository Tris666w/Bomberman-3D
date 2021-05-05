#include "stdafx.h"
#include "SkyBoxMaterial.h"

#include "ContentManager.h"
#include "ModelComponent.h"
#include "TextureData.h"
#include "TransformComponent.h"

SkyBoxMaterial::SkyBoxMaterial(): Material(L"Resources/Effects/Skybox.fx", L"")
{
}

SkyBoxMaterial::~SkyBoxMaterial()
{
}

void SkyBoxMaterial::SetCubeMapTexture(const std::wstring& assetFile)
{
	m_pCubeMapTexture = ContentManager::Load<TextureData>(assetFile);
	
}

void SkyBoxMaterial::LoadEffectVariables()
{
	m_pCubeMapVariable = GetEffect()->GetVariableByName("m_CubeMap")->AsShaderResource();
	if (!m_pCubeMapVariable->IsValid())
	{
		Logger::LogWarning(L"SkyBoxMaterial::LoadEffectVariables() > \'m_CubeMap\' variable not found!");
		m_pCubeMapVariable = nullptr;
	}
	
	m_pWVPVariable = GetEffect()->GetVariableByName("matWorldViewProj")->AsMatrix();
	if (!m_pWVPVariable->IsValid())
	{
		Logger::LogWarning(L"SkyBoxMaterial::LoadEffectVariables() > \'matWorldViewProj\' variable not found!");
		m_pWVPVariable = nullptr;
	}
	
}

void SkyBoxMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	using namespace DirectX;
	if (m_pCubeMapVariable && m_pCubeMapTexture)
	{
		m_pCubeMapVariable->SetResource(m_pCubeMapTexture->GetShaderResourceView());
	}

	auto& worldMatrix = pModelComponent->GetTransform()->GetWorld();
	auto& viewProjection = gameContext.pCamera->GetViewProjection();
	XMMATRIX const wvp = XMLoadFloat4x4(&worldMatrix) * XMLoadFloat4x4(&viewProjection);
	XMFLOAT4X4 worldViewProjectionMatrix{};
	XMStoreFloat4x4(&worldViewProjectionMatrix,wvp);

	m_pWVPVariable->SetMatrix(&worldViewProjectionMatrix._11);
}
