#pragma once
#include <GameScene.h>
#include "../UiButton.h"
class Text;
class SpriteComponent;

class PreGameMenu final :public GameScene
{
public:
	PreGameMenu();
	~PreGameMenu()override;
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;

private:
	void LoadMainMenu(void*);
	void BeginGame(void*);
	void SetAmountOfPLayers(void* pData);
	void CycleControllerChoosePlayer(void* pData);
	void SetInputScheme(void*);
	void InitializeMusic();

	UiButton<PreGameMenu, &PreGameMenu::BeginGame>* m_StartGameButton;
	UiButton<PreGameMenu, &PreGameMenu::LoadMainMenu>* m_MainMenuButton{};

	UiButton<PreGameMenu, &PreGameMenu::SetAmountOfPLayers>* m_PlayerAddPlusButton{};
	UiButton<PreGameMenu, &PreGameMenu::SetAmountOfPLayers>* m_PlayerAddMinusButton{};

	UiButton<PreGameMenu, &PreGameMenu::CycleControllerChoosePlayer>* m_pControllerChoosePlayerPlusButton{};
	UiButton<PreGameMenu, &PreGameMenu::CycleControllerChoosePlayer>* m_pControllerChoosePlayerMinusButton{};

	UiButton<PreGameMenu, &PreGameMenu::SetInputScheme>* m_CycleInputControls{};

	Text* m_pAmountOfPlayersText = nullptr;
	Text* m_pChoosePlayIndexText = nullptr;
	Text* m_pPlayerControlIndexText = nullptr;

	SpriteComponent* m_pSchemeSprite;

	int m_CurrentPlayerIndex = 1;
	int m_InputIndex = 0;

	FMOD::Channel* m_pChannel = nullptr;
};

