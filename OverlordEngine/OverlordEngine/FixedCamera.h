#pragma once
#include "GameObject.h"

struct GameContext;
class CameraComponent;

class FixedCamera final : public GameObject
{
public:
	FixedCamera(DirectX::XMFLOAT3 initialPos);

protected:
	void Initialize(const GameContext& gameContext) override;

private:
	DirectX::XMFLOAT3 m_InitialPos;
};
