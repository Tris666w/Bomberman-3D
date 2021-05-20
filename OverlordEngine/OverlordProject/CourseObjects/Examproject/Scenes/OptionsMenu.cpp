#include "stdafx.h"
#include "OptionsMenu.h"

#include "ModelComponent.h"
#include "SceneManager.h"
#include "SpriteFontLoader.h"
#include "TransformComponent.h"
#include "../UiButton.h"
#include "../BombermanGameSettings.h"
#include "../../../Materials/DiffuseMaterial.h"
#include "../Text.h"

OptionsMenu::OptionsMenu() :GameScene(L"Options menu"),
m_MainMenuButton(nullptr)
{
}

OptionsMenu::~OptionsMenu()
{
	BombermanGameSettings::GetInstance()->DestroyInstance();
}

void OptionsMenu::Initialize()
{
	std::wstring const fontPathSmall = L"Resources/SpriteFonts/BombermanMedium.fnt";

	std::wstring const fontPathBig = L"Resources/SpriteFonts/BombermanBig.fnt";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";
	std::wstring const soundBackwardPath = L"Resources/Sounds/BackwardSelect.wav";

	int const charSize = 64;
	m_MainMenuButton = new UiButton<OptionsMenu, &OptionsMenu::LoadMainMenu>(fontPathBig, soundBackwardPath, { 375.f,600.f }, L"Main menu", this, charSize);
	AddChild(m_MainMenuButton);


	float const buttonY = 320.f;
	float x1 = 180.f;
	float x2 = 245;
	float x3 = 325;

	auto* pText = new Text(L"Music", { x1 - charSize / 2,buttonY - 64 }, BombermanGameSettings::text_color, fontPathSmall);
	AddChild(pText);
	m_MusicVolumePlusButton = new UiButton<OptionsMenu, &OptionsMenu::ChangeMusicVolume>(fontPathBig, soundForwardPath, { x1,buttonY }, L"+", this, charSize);
	AddChild(m_MusicVolumePlusButton);
	m_pMusicText = new Text(std::to_wstring(static_cast<int>(BombermanGameSettings::GetInstance()->GetMusicVolume() * 10)), { x2,buttonY }, BombermanGameSettings::text_color, fontPathBig);
	AddChild(m_pMusicText);
	m_MusicVolumeMinusButton = new UiButton<OptionsMenu, &OptionsMenu::ChangeMusicVolume>(fontPathBig, soundBackwardPath, { x3,buttonY }, L"-", this, charSize);
	AddChild(m_MusicVolumeMinusButton);

	x1 += 700.f;
	x2 += 700.f;
	x3 += 700.f;

	pText = new Text(L"Sound", { x1 - charSize / 2,buttonY - 64 }, BombermanGameSettings::text_color, fontPathSmall);
	AddChild(pText);
	m_SoundVolumePlusButton = new UiButton<OptionsMenu, &OptionsMenu::ChangeSoundVolume>(fontPathBig, soundForwardPath, { x1,buttonY }, L"+", this, charSize);
	AddChild(m_SoundVolumePlusButton);
	m_pSoundText = new Text(std::to_wstring(static_cast<int>(BombermanGameSettings::GetInstance()->GetSoundVolume() * 10)), { x2,buttonY }, BombermanGameSettings::text_color, fontPathBig);
	AddChild(m_pSoundText);
	m_SoundVolumeMinusButton = new UiButton<OptionsMenu, &OptionsMenu::ChangeSoundVolume>(fontPathBig, soundBackwardPath, { x3,buttonY }, L"-", this, charSize);
	AddChild(m_SoundVolumeMinusButton);



	//Camera
	auto pObj = new GameObject();
	auto const gameContext = GetGameContext();

	auto const pCam = new CameraComponent();
	pObj->AddComponent(pCam);
	pObj->GetTransform()->Translate(29.f, 20.f, -39.f);
	SetActiveCamera(pCam);
	AddChild(pObj);

	//Set up menu texture
	pObj = new GameObject();

	auto const pModel = new ModelComponent(L"Resources/Meshes/Plane.ovm");
	auto const pMat = new DiffuseMaterial();
	pMat->SetDiffuseTexture(L"Resources/Textures/Menu/MenuDiffuse.png");
	auto const matID = GetGameContext().pMaterialManager->AddMaterial(pMat);
	pModel->SetMaterial(matID);

	pObj->AddComponent(pModel);
	AddChild(pObj);
	pObj->GetTransform()->Scale(1.5f, 1.f, 1.f);

	pObj->GetTransform()->Rotate(-90, 0, 0);
}

void OptionsMenu::Update()
{
	m_MainMenuButton->CheckForActivation(GetGameContext());

	float volume = BombermanGameSettings::GetInstance()->GetSoundVolume() - 0.1f;
	m_SoundVolumeMinusButton->CheckForActivation(GetGameContext(), &volume);

	volume += 0.2f;
	m_SoundVolumePlusButton->CheckForActivation(GetGameContext(), &volume);

	volume = BombermanGameSettings::GetInstance()->GetMusicVolume() - 0.1f;
	m_MusicVolumeMinusButton->CheckForActivation(GetGameContext(), &volume);

	volume += 0.2f;
	m_MusicVolumePlusButton->CheckForActivation(GetGameContext(), &volume);
}

void OptionsMenu::Draw()
{

}

void OptionsMenu::LoadMainMenu(void*)
{
	SceneManager::GetInstance()->SetActiveGameScene(L"Main menu");
}

void OptionsMenu::ChangeSoundVolume(void* pData)
{
	auto pFloat = static_cast<float*>(pData);
	float volume = *pFloat;
	Clamp(volume, 1.f, 0.f);
	BombermanGameSettings::GetInstance()->SetSoundVolume(volume);
	m_pSoundText->SetText(std::to_wstring(static_cast<int>(volume * 10)));
}

void OptionsMenu::ChangeMusicVolume(void* pData)
{
	auto pFloat = static_cast<float*>(pData);
	float volume = *pFloat;
	Clamp(volume, 1.f, 0.f);
	BombermanGameSettings::GetInstance()->SetMusicVolume(volume);
	m_pMusicText->SetText(std::to_wstring(static_cast<int>(volume * 10)));
}

