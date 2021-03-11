#pragma once
#include <GameScene.h>
class TorusPrefab;

class TutorialScene final: public GameScene
{
public:
	TutorialScene();
	virtual ~TutorialScene() = default;

	TutorialScene(const TutorialScene& other) = delete;
	TutorialScene(TutorialScene&& other) = delete;
	TutorialScene& operator=(const TutorialScene& other) = delete;
	TutorialScene& operator=(TutorialScene&& other) = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
private:
	TorusPrefab* m_pTorus1 = nullptr;
	TorusPrefab* m_pTorus2 = nullptr;
	TorusPrefab* m_pTorus3 = nullptr;

	float m_Angle1 = 0.f;
	float m_Angle2 = 0.f;
	float m_Angle3 = 0.f;
	
};

