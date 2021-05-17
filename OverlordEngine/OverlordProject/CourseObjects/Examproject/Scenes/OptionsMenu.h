#pragma once
#include <GameScene.h>
#include "../UiButton.h"

class OptionsMenu final:   public GameScene
{
public:
	OptionsMenu();
	~OptionsMenu() override;
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void LoadMainMenu();
	void IncreaseSoundVolume();
	void DecreaseSoundVolume();
	void IncreaseMusicVolume();
	void DecreaseMusicVolume();

private:
	SpriteFont* m_pFont;
	
	UiButton<OptionsMenu,&OptionsMenu::LoadMainMenu>* m_MainMenuButton{};
	UiButton<OptionsMenu,&OptionsMenu::IncreaseSoundVolume>* m_SoundVolumePlusButton{};
	UiButton<OptionsMenu,&OptionsMenu::DecreaseSoundVolume>* m_SoundVolumeMinusButton{};
	UiButton<OptionsMenu,&OptionsMenu::IncreaseMusicVolume>* m_MusicVolumePlusButton{};
	UiButton<OptionsMenu,&OptionsMenu::DecreaseMusicVolume>* m_MusicVolumeMinusButton{};
};

