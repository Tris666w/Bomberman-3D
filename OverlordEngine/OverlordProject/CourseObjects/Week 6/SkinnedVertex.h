#pragma once
#include "VertexHelper.h"

struct SkinnedVertex
{
	SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color,float blendWeight1, float blendWeight2):
	TransformedVertex(position, normal, color)
	, OriginalVertex(position, normal, color)
	,BlendWeight1(blendWeight1)
	,BlendWeight2(blendWeight2)
	{
	}

	SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color):
	TransformedVertex(position, normal, color)
	, OriginalVertex(position, normal, color)
	,BlendWeight1(1.f)
	,BlendWeight2(0.f)
	{
	}
	
	VertexPosNormCol TransformedVertex;
	VertexPosNormCol OriginalVertex;
	float BlendWeight1;
	float BlendWeight2;
	
};
