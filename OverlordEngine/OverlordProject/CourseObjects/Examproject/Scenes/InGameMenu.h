#pragma once
#include"GameScene.h"
#include "../UiButton.h"

class Text;
class InGameMenu final: public GameScene
{
public:
	InGameMenu();
	~InGameMenu() override;


protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void ContinueGame(void*);
	void LoadMainMenu(void*);
	void RestartGame(void*);
	void QuitGame(void*);

	void ChangeSoundVolume(void* pData);
	void ChangeMusicVolume(void* pData);

private:
	UiButton<InGameMenu,&InGameMenu::ContinueGame>* m_ContinueButton;
	UiButton<InGameMenu,&InGameMenu::LoadMainMenu>* m_MainMenuButton;
	UiButton<InGameMenu,&InGameMenu::RestartGame>* m_RestartButton;
	UiButton<InGameMenu,&InGameMenu::QuitGame>* m_QuitGameButton;

	UiButton<InGameMenu,&InGameMenu::ChangeSoundVolume>* m_SoundVolumePlusButton{};
	UiButton<InGameMenu,&InGameMenu::ChangeSoundVolume>* m_SoundVolumeMinusButton{};
	UiButton<InGameMenu,&InGameMenu::ChangeMusicVolume>* m_MusicVolumePlusButton{};
	UiButton<InGameMenu,&InGameMenu::ChangeMusicVolume>* m_MusicVolumeMinusButton{};

	Text* m_pSoundText = nullptr;
	Text* m_pMusicText = nullptr;

	void InitSoundButtons();
	
};

