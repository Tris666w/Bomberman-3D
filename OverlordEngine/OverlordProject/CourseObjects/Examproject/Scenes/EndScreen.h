#pragma once
#include <GameScene.h>
#include "../UiButton.h"
class EndScreen final : public GameScene
{
public:
	EndScreen(size_t winnerIndex);
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;

	void ContinueToMenu(void*);
private:
	size_t m_WinnerIndex;
	UiButton<EndScreen, &EndScreen::ContinueToMenu>* m_pContinueButton = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
	
	void CreateCamera();
	void CreateBackground();
	void InitializeMusic();
};

