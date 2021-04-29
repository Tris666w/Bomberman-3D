#pragma once
#include <GameScene.h>

class BombermanScene final :public GameScene
{
public:
	BombermanScene();
	~BombermanScene() override;
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:
	CameraComponent* m_pCamera = nullptr;
	
};

