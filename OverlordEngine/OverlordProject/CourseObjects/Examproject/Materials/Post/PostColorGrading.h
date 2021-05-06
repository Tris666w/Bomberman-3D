#pragma once
#include <PostProcessingMaterial.h>

class TextureData;
class PostColorGrading final: public PostProcessingMaterial
{
public:
	PostColorGrading();
	~PostColorGrading() override;

	/// @brief Set the contribution of the Color grading
	/// @param contribution between[0;1]
	void SetContribution(float contribution);

	/// @brief Set the Look up Texture
	/// @param assetPath the path
	void SetLUT(const std::wstring& assetPath);
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* rendertarget) override;

private:
	ID3DX11EffectShaderResourceVariable* m_pLookUpTextSRVariable;
	ID3DX11EffectShaderResourceVariable* m_pSceneMapVariable;
	ID3DX11EffectScalarVariable* m_pContributionVariable;
	
	float m_Contribution = 1.f;
	TextureData* m_pLUTText;
};

