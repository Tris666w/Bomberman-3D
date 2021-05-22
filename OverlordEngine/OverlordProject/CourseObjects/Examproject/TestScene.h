#pragma once
#include <GameScene.h>

class PowerUpBase;
class TestScene final : public GameScene
{
public:
	TestScene();
	~TestScene()override;
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	PowerUpBase* m_pRadiusPU = nullptr;
	PowerUpBase* m_pLifePU = nullptr;
	PowerUpBase* m_pBombBeltPU = nullptr;
	int m_ID;
};

