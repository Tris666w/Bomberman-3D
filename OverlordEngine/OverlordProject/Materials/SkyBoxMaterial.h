#pragma once
#include <Material.h>
class TextureData;

class SkyBoxMaterial final: public Material
{
public:
	SkyBoxMaterial();
	~SkyBoxMaterial() override;
	void SetCubeMapTexture(const std::wstring& assetFile);
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData* m_pCubeMapTexture = nullptr;
	
	ID3DX11EffectShaderResourceVariable* m_pCubeMapVariable = nullptr;	
	ID3DX11EffectMatrixVariable* m_pWVPVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pWorldVariable = nullptr;
	ID3DX11EffectVectorVariable* m_pCameraPosVariable = nullptr;
	
};

