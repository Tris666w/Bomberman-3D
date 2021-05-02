#include "stdafx.h"
#include "FixedCamera.h"
#include "TransformComponent.h"

FixedCamera::FixedCamera(DirectX::XMFLOAT3 initialPos)
	:m_InitialPos(initialPos)
{
}

void FixedCamera::Initialize(const GameContext&)
{
	AddComponent(new CameraComponent());

	GetTransform()->Translate(m_InitialPos);
}
