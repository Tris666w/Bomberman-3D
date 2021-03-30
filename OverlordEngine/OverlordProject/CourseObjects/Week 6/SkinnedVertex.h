#pragma once
#include "VertexHelper.h"

struct SkinnedVertex
{
	SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color):
	TransformedVertex(position, normal, color)
	, OriginalVertex(position, normal, color)
	{
	}
	
	VertexPosNormCol TransformedVertex;
	VertexPosNormCol OriginalVertex;
};
