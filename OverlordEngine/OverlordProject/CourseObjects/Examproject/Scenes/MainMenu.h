#pragma once
#include <GameScene.h>
#include "../UiButton.h"

class MainMenu final : public GameScene
{
public:
	MainMenu();
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;
	void GoToOptions(void*);
	void StartGame(void*);
	void QuitGame(void*);
private:
	UiButton<MainMenu,&MainMenu::StartGame>* m_StartGameButton;
	UiButton<MainMenu,&MainMenu::GoToOptions>* m_OptionsButton;
	UiButton<MainMenu,&MainMenu::QuitGame>* m_QuitGameButton;
};

