#pragma once
#include <PostProcessingMaterial.h>
class PostBloom final :public PostProcessingMaterial
{
public:
	PostBloom();
	virtual ~PostBloom() = default;

	/// @brief Set the intensity of the bloom effect 
	/// @param intensity between [0;1]
	void SetIntensity(float intensity);
	
	PostBloom(const PostBloom& other) = delete;
	PostBloom(PostBloom&& other) noexcept = delete;
	PostBloom& operator=(const PostBloom& other) = delete;
	PostBloom& operator=(PostBloom&& other) noexcept = delete;
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariable;
	ID3DX11EffectScalarVariable* m_pIntensityVariable;

	float m_Intensity = 1.f;
};

