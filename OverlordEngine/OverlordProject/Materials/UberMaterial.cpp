#include "stdafx.h"

#include "UberMaterial.h"

#include "TextureData.h"
#include "ContentManager.h"
#include "utils.h"

ID3DX11EffectVectorVariable* UberMaterial::m_pLightDirectionVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseDiffuseTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pDiffuseColorVariable = nullptr;

ID3DX11EffectVectorVariable* UberMaterial::m_pSpecularColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseSpecularLevelTextureVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pSpecularLevelSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pShininessVariable = nullptr;

ID3DX11EffectVectorVariable* UberMaterial::m_pAmbientColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pAmbientIntensityVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pFlipGreenChannelVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseNormalMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pNormalMappingSRVvariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseEnvironmentMappingVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pEnvironmentSRVvariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pReflectionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionStrengthVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pRefractionIndexVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pOpacityVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUseOpacityMapVariable = nullptr;
ID3DX11EffectShaderResourceVariable* UberMaterial::m_pOpacitySRVvariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseBlinnVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pUsePhongVariable = nullptr;

ID3DX11EffectScalarVariable* UberMaterial::m_pUseFresnelFalloffVariable = nullptr;
ID3DX11EffectVectorVariable* UberMaterial::m_pFresnelColorVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelPowerVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelMultiplierVariable = nullptr;
ID3DX11EffectScalarVariable* UberMaterial::m_pFresnelHardnessVariable = nullptr;


UberMaterial::UberMaterial() : Material(L"Resources/Effects/UberShader.fx", L"WithAlphaBlending")
	,m_LightDirection(0.577f, 0.577f, 0.577f)
	,m_bDiffuseTexture(false)
	,m_pDiffuseTexture(nullptr)
	,m_ColorDiffuse({1.f,1.f,1.f,1.f})
	,m_ColorSpecular({1.f,1.f,1.f,1.f})
	,m_bSpecularLevelTexture(false)
	,m_pSpecularLevelTexture(nullptr)
	,m_Shininess(10)
	,m_ColorAmbient({1.f,1.f,1.f,1.f})
	,m_AmbientIntensity(0.1f)
	,m_bFlipGreenChannel(false)
	,m_bNormalMapping(false)
	,m_pNormalMappingTexture(nullptr)
	,m_bEnvironmentMapping(false)
	,m_pEnvironmentCube(nullptr)
	,m_ReflectionStrength(0.5f)
	,m_RefractionStrength(0.5f)
	,m_RefractionIndex(1.f)
	,m_Opacity(1.f)
	,m_bOpacityMap(false)
	,m_pOpacityMap(nullptr)
	,m_bSpecularBlinn(false)
	,m_bSpecularPhong(false)
	,m_bFresnelFaloff(false)
	,m_ColorFresnel({1.f,1.f,1.f,1.f})
	,m_FresnelPower(1.f)
	,m_FresnelMultiplier(1.f)
	,m_FresnelHardness(1.f)
{
	
}

UberMaterial::~UberMaterial()
{
	
}

void UberMaterial::SetLightDirection(const DirectX::XMFLOAT3 direction)
{
	m_LightDirection = direction;
}

void UberMaterial::EnableDiffuseTexture(const bool enable)
{
	m_bDiffuseTexture = enable;
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	utils::SafeDelete(m_pDiffuseTexture);
	
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
 }

void UberMaterial::SetDiffuseColor(const DirectX::XMFLOAT4 color)
{
	m_ColorDiffuse = color;
}

void UberMaterial::SetSpecularColor(const DirectX::XMFLOAT4 color)
{
	m_ColorAmbient = color;
}

void UberMaterial::EnableSpecularLevelTexture(const bool enable)
{
	m_bSpecularLevelTexture = enable;
}

void UberMaterial::SetSpecularLevelTexture(const std::wstring& assetFile)
{
	utils::SafeDelete(m_pSpecularLevelTexture);

	
	m_pSpecularLevelTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetShininess(const int shininess)
{
	m_Shininess = shininess;
}

void UberMaterial::SetAmbientColor(const DirectX::XMFLOAT4 color)
{
	m_ColorAmbient = color;
}

void UberMaterial::SetAmbientIntensity(const float intensity)
{
	m_AmbientIntensity = intensity;
}

void UberMaterial::FlipNormalGreenChannel(const bool flip)
{
	m_bFlipGreenChannel = flip;
}

void UberMaterial::EnableNormalMapping(const bool enable)
{
	m_bNormalMapping = enable;
}

void UberMaterial::SetNormalMapTexture(const std::wstring& assetFile)
{
	utils::SafeDelete(m_pNormalMappingTexture);
	
	m_pNormalMappingTexture = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableEnvironmentMapping(const bool enable)
{
	m_bEnvironmentMapping = enable;
}

void UberMaterial::SetEnvironmentCube(const std::wstring& assetFile)
{
	utils::SafeDelete(m_pEnvironmentCube);
	
	m_pEnvironmentCube = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::SetReflectionStrength(const float strength)
{
	m_ReflectionStrength = strength;
}

void UberMaterial::SetRefractionStrength(const float strength)
{
	m_RefractionStrength = strength;
}

void UberMaterial::SetRefractionIndex(const float index)
{
	m_RefractionIndex = index;
}

void UberMaterial::SetOpacity(const float opacity)
{
	m_Opacity = opacity;
}

void UberMaterial::EnableOpacityMap(const bool enable)
{
	m_bOpacityMap = enable;
}

void UberMaterial::SetOpacityTexture(const std::wstring& assetFile)
{
	utils::SafeDelete(m_pOpacityMap);
	
	m_pOpacityMap = ContentManager::Load<TextureData>(assetFile);
}

void UberMaterial::EnableSpecularBlinn(const bool enable)
{
	m_bSpecularBlinn = enable;
}

void UberMaterial::EnableSpecularPhong(const bool enable)
{
	m_bSpecularPhong = enable;
}

void UberMaterial::EnableFresnelFaloff(const bool enable)
{
	m_bFresnelFaloff = enable;
}


void UberMaterial::SetFresnelColor(const DirectX::XMFLOAT4 color)
{
	m_ColorFresnel = color;
}

void UberMaterial::SetFresnelPower(const float power)
{
	m_FresnelPower = power;
}

void UberMaterial::SetFresnelMultiplier(const float multiplier)
{
	m_FresnelMultiplier = multiplier;
}

void UberMaterial::SetFresnelHardness(const float hardness)
{
	m_FresnelHardness = hardness;
}

void UberMaterial::LoadEffectVariables()
{
	//LIGHT
	//*****
	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pLightDirectionVariable->IsValid())
	{
		std::wcout << L"m_pLightDirectionVariable not valid\n";
	}

	//DIFFUSE
	//*******
	m_pUseDiffuseTextureVariable = GetEffect()->GetVariableByName("gUseTextureDiffuse")->AsScalar();
	if (!m_pUseDiffuseTextureVariable->IsValid())
	{
		std::wcout << L"m_pUseDiffuseTextureVariable not valid\n";
	}

	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gTextureDiffuse")->AsShaderResource();
	if (!m_pDiffuseSRVvariable->IsValid())
	{
		std::wcout << L"m_pDiffuseSRVvariable not valid\n";
	}

	m_pDiffuseColorVariable = GetEffect()->GetVariableByName("gColorDiffuse")->AsVector();
	if (!m_pDiffuseColorVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseColorVariable not valid\n";
	}

	//SPECULAR
	//********
	m_pSpecularColorVariable = GetEffect()->GetVariableByName("gColorSpecular")->AsVector();
	if (!m_pSpecularColorVariable->IsValid())
	{
		std::wcout << L"m_pSpecularColorVariable not valid\n";
	}
	
	m_pUseSpecularLevelTextureVariable = GetEffect()->GetVariableByName("gUseTextureSpecularIntensity")->AsScalar();
	if (!m_pUseSpecularLevelTextureVariable->IsValid())
	{
		std::wcout << L"m_pUseSpecularLevelTextureVariable not valid\n";
	}

	m_pSpecularLevelSRVvariable = GetEffect()->GetVariableByName("gTextureSpecularIntensity")->AsShaderResource();
	if (!m_pSpecularLevelSRVvariable->IsValid())
	{
		std::wcout << L"m_pSpecularLevelSRVvariable not valid\n";
	}

	m_pShininessVariable = GetEffect()->GetVariableByName("gShininess")->AsScalar();
	if (!m_pShininessVariable->IsValid())
	{
		std::wcout << L"m_pShininessVariable not valid\n";
	}

	//AMBIENT
	//*******
	m_pAmbientColorVariable = GetEffect()->GetVariableByName("gColorAmbient")->AsVector();
	if (!m_pAmbientColorVariable->IsValid())
	{
		std::wcout << L"m_pAmbientColorVariable not valid\n";
	}
	
	m_pAmbientIntensityVariable = GetEffect()->GetVariableByName("gAmbientIntensity")->AsScalar();
	if (!m_pAmbientIntensityVariable->IsValid())
	{
		std::wcout << L"m_pAmbientIntensityVariable not valid\n";
	}

	//NORMAL MAPPING
	//**************
	m_pFlipGreenChannelVariable = GetEffect()->GetVariableByName("gFlipGreenChannel")->AsScalar();
	if (!m_pFlipGreenChannelVariable->IsValid())
	{
		std::wcout << L"m_pFlipGreenChannelVariable not valid\n";
	}

	m_pUseNormalMappingVariable = GetEffect()->GetVariableByName("gUseTextureNormal")->AsScalar();
	if (!m_pUseNormalMappingVariable->IsValid())
	{
		std::wcout << L"m_pUseNormalMappingVariable not valid\n";
	}

	m_pNormalMappingSRVvariable = GetEffect()->GetVariableByName("gTextureNormal")->AsShaderResource();
	if (!m_pNormalMappingSRVvariable->IsValid())
	{
		std::wcout << L"m_pNormalMappingSRVvariable not valid\n";
	}

	//ENVIRONMENT MAPPING
	//*******************
	m_pUseEnvironmentMappingVariable = GetEffect()->GetVariableByName("gUseEnvironmentMapping")->AsScalar();
	if (!m_pUseEnvironmentMappingVariable->IsValid())
	{
		std::wcout << L"m_pUseEnvironmentMappingVariable not valid\n";
	}
	
	m_pEnvironmentSRVvariable = GetEffect()->GetVariableByName("gCubeEnvironment")->AsShaderResource();
	if (!m_pEnvironmentSRVvariable->IsValid())
	{
		std::wcout << L"m_pEnvironmentSRVvariable not valid\n";
	}

	m_pReflectionStrengthVariable = GetEffect()->GetVariableByName("gReflectionStrength")->AsScalar();
	if (!m_pReflectionStrengthVariable->IsValid())
	{
		std::wcout << L"m_pReflectionStrengthVariable not valid\n";
	}

	m_pRefractionStrengthVariable = GetEffect()->GetVariableByName("gRefractionStrength")->AsScalar();
	if (!m_pRefractionStrengthVariable->IsValid())
	{
		std::wcout << L"m_pRefractionStrengthVariable not valid\n";
	}

	m_pRefractionIndexVariable = GetEffect()->GetVariableByName("gRefractionIndex")->AsScalar();
	if (!m_pRefractionIndexVariable->IsValid())
	{
		std::wcout << L"m_pRefractionIndexVariable not valid\n";
	}

	//OPACITY
	//***************
	m_pOpacityVariable = GetEffect()->GetVariableByName("gOpacityIntensity")->AsScalar();
	if (!m_pOpacityVariable->IsValid())
	{
		std::wcout << L"m_pOpacityVariable not valid\n";
	}

	m_pUseOpacityMapVariable = GetEffect()->GetVariableByName("gTextureOpacityIntensity")->AsScalar();
	if (!m_pUseOpacityMapVariable->IsValid())
	{
		std::wcout << L"m_pUseOpacityMapVariable not valid\n";
	}

	m_pOpacitySRVvariable = GetEffect()->GetVariableByName("gTextureOpacity")->AsShaderResource();
	if (!m_pOpacitySRVvariable->IsValid())
	{
		std::wcout << L"m_pOpacitySRVvariable not valid\n";
	}

	//SPECULAR MODELS
	//***************
	m_pUseBlinnVariable = GetEffect()->GetVariableByName("gUseSpecularBlinn")->AsScalar();
	if (!m_pUseBlinnVariable->IsValid())
	{
		std::wcout << L"m_pUseBlinnVariable not valid\n";
	}

	m_pUsePhongVariable = GetEffect()->GetVariableByName("gUseSpecularPhong")->AsScalar();
	if (!m_pUsePhongVariable->IsValid())
	{
		std::wcout << L"m_pUsePhongVariable not valid\n";
	}

	//FRESNEL FALLOFF
	//***************
	m_pUseFresnelFalloffVariable = GetEffect()->GetVariableByName("gUseFresnelFalloff")->AsScalar();
	if (!m_pUseFresnelFalloffVariable->IsValid())
	{
		std::wcout << L"m_pUseFresnelFalloffVariable not valid\n";
	}

	m_pFresnelColorVariable = GetEffect()->GetVariableByName("gColorFresnel")->AsVector();
	if (!m_pFresnelColorVariable->IsValid())
	{
		std::wcout << L"m_pFresnelColorVariable not valid\n";
	}

	m_pFresnelPowerVariable = GetEffect()->GetVariableByName("gFresnelPower")->AsScalar();
	if (!m_pFresnelPowerVariable->IsValid())
	{
		std::wcout << L"m_pFresnelPowerVariable not valid\n";
	}

	m_pFresnelMultiplierVariable = GetEffect()->GetVariableByName("gFresnelMultiplier")->AsScalar();
	if (!m_pFresnelMultiplierVariable->IsValid())
	{
		std::wcout << L"m_pFresnelMultiplierVariable not valid\n";
	}

	m_pFresnelHardnessVariable = GetEffect()->GetVariableByName("gFresnelHardness")->AsScalar();
	if (!m_pFresnelHardnessVariable->IsValid())
	{
		std::wcout << L"m_pFresnelHardnessVariable not valid\n";
	}
	
}

void UberMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{

	//LIGHT
	//*****
	m_pLightDirectionVariable->SetFloatVector(&m_LightDirection.x);

	//DIFFUSE
	//*******
	m_pUseDiffuseTextureVariable->SetBool(m_bDiffuseTexture);
	if (m_pDiffuseTexture)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	m_pDiffuseColorVariable->SetFloatVector(&m_ColorDiffuse.x);

	//SPECULAR
	//********
	m_pSpecularColorVariable->SetFloatVector(&m_ColorSpecular.x);
	m_pUseSpecularLevelTextureVariable->SetBool(m_bSpecularLevelTexture);
	if (m_pSpecularLevelTexture)
		m_pSpecularLevelSRVvariable->SetResource(m_pSpecularLevelTexture->GetShaderResourceView());
	m_pShininessVariable->SetBool(m_Shininess);

	//AMBIENT
	//*******
	m_pAmbientColorVariable->SetFloatVector(&m_ColorAmbient.x);
	m_pAmbientIntensityVariable->SetFloat(m_AmbientIntensity);

	//NORMAL MAPPING
	//**************
	m_pFlipGreenChannelVariable->SetBool(m_bFlipGreenChannel);
	m_pUseNormalMappingVariable->SetBool(m_bNormalMapping);
	if(m_pNormalMappingTexture)
		m_pNormalMappingSRVvariable->SetResource(m_pNormalMappingTexture->GetShaderResourceView());
	
	//ENVIRONMENT MAPPING
	//*******************
	m_pUseEnvironmentMappingVariable->SetBool(m_bEnvironmentMapping);
	if (m_pEnvironmentCube)
		m_pEnvironmentSRVvariable->SetResource(m_pEnvironmentCube->GetShaderResourceView());
	m_pReflectionStrengthVariable->SetFloat(m_ReflectionStrength);
	m_pRefractionStrengthVariable->SetFloat(m_RefractionStrength);
	m_pRefractionIndexVariable->SetFloat(m_RefractionIndex);

	//OPACITY
	//***************
	m_pOpacityVariable->SetFloat(m_Opacity);
	m_pUseOpacityMapVariable->SetBool(m_bOpacityMap);
	if (m_pOpacityMap)
		m_pOpacitySRVvariable->SetResource(m_pOpacityMap->GetShaderResourceView());

	//SPECULAR MODELS
	//***************
	m_pUseBlinnVariable->SetBool(m_bSpecularBlinn);
	m_pUsePhongVariable->SetBool(m_bSpecularPhong);

	//FRESNEL FALLOFF
	//***************
	m_pUseFresnelFalloffVariable->SetBool(m_bFresnelFaloff);
	m_pFresnelColorVariable->SetFloatVector(&m_ColorFresnel.x);
	m_pFresnelPowerVariable->SetFloat(m_FresnelPower);
	m_pFresnelMultiplierVariable->SetFloat(m_FresnelMultiplier);
	m_pFresnelHardnessVariable->SetFloat(m_FresnelHardness);
	
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
}
