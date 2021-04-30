#pragma once

namespace helpers{
	#pragma region
	inline DirectX::XMFLOAT4 RGBADivide(float r, float g,float b, float a)
	{
		return DirectX::XMFLOAT4{r / 255.f, g/ 255.f,b/255.f,a/255.f};
	}
	#pragma endregion prototypeHelper
}