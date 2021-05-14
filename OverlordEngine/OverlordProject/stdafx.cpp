#include "stdafx.h"

bool IsPointInRect(const POINT& point, const DirectX::Rect& rect)
{
	DirectX::XMFLOAT2 pos = { static_cast<float>(point.x),static_cast<float>(point.y) };
	return (pos.x >= rect.x &&
		pos.x <= rect.x + rect.w &&
		pos.y >= rect.y &&
		pos.y <= rect.y + rect.h);
}
