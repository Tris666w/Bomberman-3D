//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	if (!m_IsInitialized)
	{
		//TODO: initialize the effect, techniques, shader variables, input layouts (hint use EffectHelper::BuildInputLayout), etc.
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	UNREFERENCED_PARAMETER(lightVP);
	//TODO: set the correct shader variable
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	UNREFERENCED_PARAMETER(world);
	//TODO: set the correct shader variable
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	UNREFERENCED_PARAMETER(pData);
	UNREFERENCED_PARAMETER(count);
	//TODO: set the correct shader variable
}
