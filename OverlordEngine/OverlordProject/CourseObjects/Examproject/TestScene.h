#pragma once
#include <GameScene.h>
class TestScene final : public GameScene
{
public:
	TestScene();
	~TestScene()override;
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
};

