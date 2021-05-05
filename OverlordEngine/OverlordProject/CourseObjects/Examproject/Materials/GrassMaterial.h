#pragma once
#include <Material.h>
class TextureData;

class GrassMaterial final :public Material
{
public:
	GrassMaterial(const std::wstring& effectPath);
	~GrassMaterial()override;

	void SetLightDirection(DirectX::XMFLOAT3 direction);

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetHeightTexture(const std::wstring& assetFile);
	void SetDirectionTexture(const std::wstring& assetFile);
	void SetWeightTexture(const std::wstring& assetFile);
	void SetAddedColorTexture(const std::wstring& assetFile);
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData* m_pDiffuseTexture = nullptr;
	TextureData* m_pHeightTexture = nullptr;
	TextureData* m_pDirectionTexture = nullptr;
	TextureData* m_pWeightTexture = nullptr;
	TextureData* m_pAddedColorTexture = nullptr;
	DirectX::XMFLOAT3 m_LightDirection = {};
	bool m_CheckBools = false;
	static ID3DX11EffectScalarVariable* m_pElapsedTimeVariable;
	static ID3DX11EffectVectorVariable* m_pLightDirectionVariable;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVariable;
	static ID3DX11EffectShaderResourceVariable* m_pHeightMapSRVariable;
	static ID3DX11EffectShaderResourceVariable* m_pDirectionMapSRVariable;
	static ID3DX11EffectShaderResourceVariable* m_pWeightSRVariable;
	static ID3DX11EffectShaderResourceVariable* m_pAddedColorSRVariable;

	void CheckBooleans();
};

