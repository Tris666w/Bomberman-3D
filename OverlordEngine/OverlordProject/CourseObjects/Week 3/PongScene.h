#pragma once
#include <GameScene.h>
class SpherePrefab;

class PongScene final: public GameScene
{
public:
	PongScene();
	virtual ~PongScene() = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	GameObject* m_pLeftPaddle = nullptr;
	GameObject* m_pRightPaddle = nullptr;
	const float m_PeddleSpeed = 5.f;

	SpherePrefab* m_pSphere = nullptr;
	bool m_CanResetSphere = false;
	
	void ResetSphere();
	enum PongInput
	{
		LeftPeddleUp = 0,
		LeftPeddleDown = 1,
		RightPeddleUp = 2,
		RightPeddleDown = 3,
		LaunchBall = 4
	};
};

