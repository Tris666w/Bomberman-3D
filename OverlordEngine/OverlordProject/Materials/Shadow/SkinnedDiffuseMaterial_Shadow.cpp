//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SkinnedDiffuseMaterial_Shadow.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pBoneTransforms = nullptr;
ID3DX11EffectVectorVariable* SkinnedDiffuseMaterial_Shadow::m_pLightDirectionVariable = nullptr;
ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial_Shadow::m_pShadowSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial_Shadow::m_pLightWVPvariable = nullptr;

SkinnedDiffuseMaterial_Shadow::SkinnedDiffuseMaterial_Shadow() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Skinned_Shadow.fx"),
	m_pDiffuseTexture(nullptr)
{}

void SkinnedDiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture =ContentManager::Load<TextureData>(assetFile);
	
}

void SkinnedDiffuseMaterial_Shadow::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	m_LightDirection = dir;

}

void SkinnedDiffuseMaterial_Shadow::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial_Shadow::LoadEffectVariables() > \'gDiffuseMap\' variable not found!");
		m_pDiffuseSRVvariable = nullptr;
	}
	m_pShadowSRVvariable = GetEffect()->GetVariableByName("gShadowMap")->AsShaderResource();
	if (!m_pShadowSRVvariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial_Shadow::LoadEffectVariables() > \'gShadowMap\' variable not found!");
		m_pShadowSRVvariable = nullptr;
	}
	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pLightDirectionVariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial_Shadow::LoadEffectVariables() > \'gLightDirection\' variable not found!");
		m_pLightDirectionVariable = nullptr;
	}
	m_pLightWVPvariable = GetEffect()->GetVariableByName("gWorldViewProj_Light")->AsMatrix();
	if (!m_pLightWVPvariable->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial_Shadow::LoadEffectVariables() > \'gWorldViewProj_Light\' variable not found!");
		m_pLightWVPvariable = nullptr;
	}
	m_pBoneTransforms = GetEffect()->GetVariableByName("gBones")->AsMatrix();
	if (!m_pBoneTransforms->IsValid())
	{
		Logger::LogWarning(L"DiffuseMaterial_Shadow::LoadEffectVariables() > \'gBones\' variable not found!");
		m_pBoneTransforms = nullptr;
	}
}

void SkinnedDiffuseMaterial_Shadow::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	}
	
	m_pLightDirectionVariable->SetFloatVector(&m_LightDirection.x);

	using namespace DirectX;
	XMMATRIX const world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
	auto const lightVP = gameContext.pShadowMapper->GetLightVP();
	XMMATRIX const vp = XMLoadFloat4x4(&lightVP);
	XMMATRIX const wvp = world * vp;
	XMFLOAT4X4 lightWorldViewProjection{};
	XMStoreFloat4x4(&lightWorldViewProjection,wvp);
	m_pLightWVPvariable->SetMatrix(&lightWorldViewProjection._11);
	
	m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());
	auto boneTransforms = pModelComponent->GetAnimator()->GetBoneTransforms();
	m_pBoneTransforms->SetMatrixArray(&boneTransforms[0]._11,0,boneTransforms.size());
}