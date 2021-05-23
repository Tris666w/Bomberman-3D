#include "stdafx.h"
#include "MainMenu.h"

#include "BombermanScene.h"
#include "ModelComponent.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "..\UiButton.h"
#include "../../../Materials/DiffuseMaterial.h"
#include "../Text.h"

MainMenu::MainMenu() :GameScene(L"Main menu"),
m_StartGameButton(nullptr),
m_QuitGameButton(nullptr)

{
}


void MainMenu::Initialize()
{
	std::wstring const fontPath = L"Resources/SpriteFonts/BombermanMedium.fnt";
	std::wstring const fontPathBig = L"Resources/SpriteFonts/BombermanBig.fnt";

	std::wstring const soundBackwardPath = L"Resources/Sounds/BackwardSelect.wav";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";


	m_StartGameButton = new UiButton<MainMenu, &MainMenu::StartGame>(fontPath, soundForwardPath, { 450,200.f }, L"Start Game", this, 64);
	AddChild(m_StartGameButton);

	m_OptionsButton = new UiButton<MainMenu, &MainMenu::GoToOptions>(fontPath, soundForwardPath, { 475,350.f }, L"Options", this, 64);
	AddChild(m_OptionsButton);

	m_QuitGameButton = new UiButton<MainMenu, &MainMenu::QuitGame>(fontPath, soundBackwardPath, { 450,500.f }, L"Quit Game", this, 64);
	AddChild(m_QuitGameButton);

	auto pText = new Text(L"BomberSquad", { 375.f,100.f }, BombermanGameSettings::text_color, fontPathBig);
	AddChild(pText);

	//Camera
	auto pObj = new GameObject();
	auto& gameContext = GetGameContext();
	auto const input = InputAction(2, InputTriggerState::Pressed, 'R');
	gameContext.pInput->AddInputAction(input);

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

	InitializeMusic();
}


void MainMenu::Update()
{
	m_StartGameButton->CheckForActivation(GetGameContext());
	m_OptionsButton->CheckForActivation(GetGameContext());
	m_QuitGameButton->CheckForActivation(GetGameContext());
}

void MainMenu::Draw()
{
}

void MainMenu::SceneActivated()
{
	GetGameContext().pInput->Initialize();

	if (m_pChannel)
	{
		m_pChannel->setPaused(false);
		m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetMusicVolume());
	}
}

void MainMenu::SceneDeactivated()
{
	if (m_pChannel)
		m_pChannel->setPaused(true);
}

void MainMenu::GoToOptions(void*)
{
	SceneManager::GetInstance()->SetActiveGameScene(L"Options menu");
}

void MainMenu::StartGame(void*)
{
	SceneManager::GetInstance()->SetActiveGameScene(L"Pre game menu");
}

void MainMenu::QuitGame(void*)
{
	PostQuitMessage(0);
}

void MainMenu::InitializeMusic()
{
	FMOD::Sound* pSound;
	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/Menu.wav", FMOD_DEFAULT, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_NORMAL);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);

	SoundManager::GetInstance()->GetSystem()->playSound(pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetMusicVolume());
	m_pChannel->setPaused(true);
}
