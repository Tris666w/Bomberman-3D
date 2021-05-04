#include "stdafx.h"
#include "FixedCamera.h"
#include "TransformComponent.h"

FixedCamera::FixedCamera(DirectX::XMFLOAT3 initialPos)
	:m_InitialPos(initialPos)
	, m_pCameraComponent(nullptr)
{
}

void FixedCamera::Initialize(const GameContext&)
{
	m_pCameraComponent = new CameraComponent();
	AddComponent(m_pCameraComponent);

	GetTransform()->Translate(m_InitialPos);
}
