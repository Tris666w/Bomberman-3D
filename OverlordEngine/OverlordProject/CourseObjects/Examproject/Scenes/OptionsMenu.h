#pragma once
#include <GameScene.h>
#include "../UiButton.h"

class Text;
class OptionsMenu final:   public GameScene
{
public:
	OptionsMenu();
	~OptionsMenu() override;
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void LoadMainMenu(void* pData);
	void ChangeSoundVolume(void* pData);
	void ChangeMusicVolume(void* pData);

private:
	UiButton<OptionsMenu,&OptionsMenu::LoadMainMenu>* m_MainMenuButton{};
	UiButton<OptionsMenu,&OptionsMenu::ChangeSoundVolume>* m_SoundVolumePlusButton{};
	UiButton<OptionsMenu,&OptionsMenu::ChangeSoundVolume>* m_SoundVolumeMinusButton{};
	UiButton<OptionsMenu,&OptionsMenu::ChangeMusicVolume>* m_MusicVolumePlusButton{};
	UiButton<OptionsMenu,&OptionsMenu::ChangeMusicVolume>* m_MusicVolumeMinusButton{};

	Text* m_pSoundText = nullptr;
	Text* m_pMusicText = nullptr;
	
};

