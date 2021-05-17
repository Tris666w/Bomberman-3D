#include "stdafx.h"
#include "OptionsMenu.h"

#include "ModelComponent.h"
#include "SceneManager.h"
#include "SpriteFontLoader.h"
#include "TransformComponent.h"
#include "../UiButton.h"
#include "../BombermanGameSettings.h"
#include "../../../Materials/DiffuseMaterial.h"

OptionsMenu::OptionsMenu():GameScene(L"Options menu"),
	m_MainMenuButton(nullptr)
{
}

OptionsMenu::~OptionsMenu()
{
	BombermanGameSettings::GetInstance()->DestroyInstance();
}

void OptionsMenu::Initialize()
{
	
	std::wstring const fontPath = L"Resources/SpriteFonts/BombermanBig.fnt";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";
	std::wstring const soundBackwardPath = L"Resources/Sounds/BackwardSelect.wav";
	
	m_pFont = ContentManager::Load<SpriteFont>(fontPath);
	
	m_MainMenuButton = new UiButton<OptionsMenu, &OptionsMenu::LoadMainMenu>(fontPath, soundBackwardPath, { 375.f,600.f }, L"Main menu", this, 64);
	AddChild(m_MainMenuButton);

	m_SoundVolumePlusButton = new UiButton<OptionsMenu, &OptionsMenu::IncreaseSoundVolume>(fontPath, soundForwardPath, { 180,220 }, L"+", this, 64);
	AddChild(m_SoundVolumePlusButton);
	m_MusicVolumePlusButton = new UiButton<OptionsMenu, &OptionsMenu::IncreaseMusicVolume>(fontPath, soundForwardPath, { 180,320 }, L"+", this, 64);
	AddChild(m_MusicVolumePlusButton);

	m_SoundVolumeMinusButton = new UiButton<OptionsMenu, &OptionsMenu::DecreaseSoundVolume>(fontPath, soundBackwardPath, { 325,220 }, L"-", this, 64);
	AddChild(m_SoundVolumeMinusButton);
	m_MusicVolumeMinusButton = new UiButton<OptionsMenu, &OptionsMenu::DecreaseMusicVolume>(fontPath, soundBackwardPath, { 325,320 }, L"-", this, 64);
	AddChild(m_MusicVolumeMinusButton);
	
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

	auto const input = InputAction(1000,InputTriggerState::Pressed,-1,VK_RBUTTON);
	gameContext.pInput->AddInputAction(input);
}

void OptionsMenu::Update()
{
	m_MainMenuButton->CheckForActivation(GetGameContext());
	m_SoundVolumeMinusButton->CheckForActivation(GetGameContext());
	m_SoundVolumePlusButton->CheckForActivation(GetGameContext());
	m_MusicVolumeMinusButton->CheckForActivation(GetGameContext());
	m_MusicVolumePlusButton->CheckForActivation(GetGameContext());
	
	auto& gameContext = GetGameContext();
	if (gameContext.pInput->IsActionTriggered(1000))
	{
		auto const mousePos = gameContext.pInput->GetMousePosition();
		Logger::LogInfo(L"Mouse coordinates are: x = "+std::to_wstring(mousePos.x)+L", y= "+std::to_wstring(mousePos.y));
	}
}

void OptionsMenu::Draw()
{
	TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(static_cast<int>(BombermanGameSettings::GetInstance()->GetSoundVolume()*10)), 
	{245,220  },static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Red));
	TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(static_cast<int>(BombermanGameSettings::GetInstance()->GetMusicVolume()*10)), 
	{245,320  },static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Red));
}

void OptionsMenu::LoadMainMenu()
{
	SceneManager::GetInstance()->SetActiveGameScene(L"Main menu");
}

void OptionsMenu::IncreaseSoundVolume()
{
	auto volume =	BombermanGameSettings::GetInstance()->GetSoundVolume();
	volume+=0.1f;
	Clamp(volume,1.f,0.f);
	BombermanGameSettings::GetInstance()->SetSoundVolume(volume);
	Logger::LogInfo(std::to_wstring(volume));
}

void OptionsMenu::DecreaseSoundVolume()
{
	auto volume =	BombermanGameSettings::GetInstance()->GetSoundVolume();
	volume-=0.1f;
	Clamp(volume,1.f,0.f);
	BombermanGameSettings::GetInstance()->SetSoundVolume(volume);
	Logger::LogInfo(std::to_wstring(volume));
}

void OptionsMenu::IncreaseMusicVolume()
{
	auto volume =	BombermanGameSettings::GetInstance()->GetMusicVolume();
	volume+=0.1f;
	Clamp(volume,1.f,0.f);
	BombermanGameSettings::GetInstance()->SetMusicVolume(volume);
	Logger::LogInfo(std::to_wstring(volume));
}

void OptionsMenu::DecreaseMusicVolume()
{
		auto volume =	BombermanGameSettings::GetInstance()->GetMusicVolume();
	volume-=0.1f;
	Clamp(volume,1.f,0.f);
	BombermanGameSettings::GetInstance()->SetMusicVolume(volume);
	Logger::LogInfo(std::to_wstring(volume));
}
