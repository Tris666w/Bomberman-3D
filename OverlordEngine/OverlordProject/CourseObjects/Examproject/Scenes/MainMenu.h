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
private:
	UiButton<MainMenu,&MainMenu::Print>* pButton;
};

