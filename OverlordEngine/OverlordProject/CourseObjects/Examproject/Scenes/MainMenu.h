#pragma once
#include <GameScene.h>
#include "..\UiButton.h"

class MainMenu final : public GameScene
{
public:
	MainMenu();
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Print();
	void StartGame();
	void QuitGame();
private:
	UiButton<MainMenu,&MainMenu::Print>* pButton;
	UiButton<MainMenu,&MainMenu::StartGame>* m_StartGameButton;
	UiButton<MainMenu,&MainMenu::QuitGame>* m_QuitGameButton;

};

