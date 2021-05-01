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
	m_pCubeMapVariable = GetEffect()->GetVariableByName("gTextureEnv")->AsShaderResource();
	if (!m_pCubeMapVariable->IsValid())
	{
		Logger::LogWarning(L"SkyBoxMaterial::LoadEffectVariables() > \'gTextureEnv\' variable not found!");
		m_pCubeMapVariable = nullptr;
	}
	
	m_pWVPVariable = GetEffect()->GetVariableByName("gMatrixWVP")->AsMatrix();
	if (!m_pWVPVariable->IsValid())
	{
		Logger::LogWarning(L"SkyBoxMaterial::LoadEffectVariables() > \'gMatrixWVP\' variable not found!");
		m_pWVPVariable = nullptr;
	}
	
	m_pWorldVariable = GetEffect()->GetVariableByName("gWorld")->AsMatrix();
	if (!m_pWorldVariable->IsValid())
	{
		Logger::LogWarning(L"SkyBoxMaterial::LoadEffectVariables() > \'gWorld\' variable not found!");
		m_pWorldVariable = nullptr;
	}
	
	m_pCameraPosVariable = GetEffect()->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraPosVariable->IsValid())
	{
		Logger::LogWarning(L"SkyBoxMaterial::LoadEffectVariables() > \'gCameraPosition\' variable not found!");
		m_pCameraPosVariable = nullptr;
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
	m_pWorldVariable->SetMatrix(&worldMatrix._11);
	m_pCameraPosVariable->SetFloatVector(&gameContext.pCamera->GetTransform()->GetPosition().x);
}
