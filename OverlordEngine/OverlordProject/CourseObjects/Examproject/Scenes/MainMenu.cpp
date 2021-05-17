#include "stdafx.h"
#include "MainMenu.h"

#include "BombermanScene.h"
#include "ModelComponent.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "..\UiButton.h"
#include "../../../Materials/DiffuseMaterial.h"

MainMenu::MainMenu() :GameScene(L"Main menu"),
	m_StartGameButton(nullptr),
	m_QuitGameButton(nullptr)

{
}

void MainMenu::StartGame()
{
	SceneManager::GetInstance()->AddGameScene(new BombermanScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"BombermanScene");
}

void MainMenu::QuitGame()
{
	PostQuitMessage(0);
}

void MainMenu::Initialize()
{
	std::wstring const fontPath = L"Resources/SpriteFonts/BombermanBig.fnt";
	std::wstring const soundBackwardPath = L"Resources/Sounds/BackwardSelect.wav";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";

	m_StartGameButton = new UiButton<MainMenu, &MainMenu::StartGame>(fontPath, soundForwardPath, { 375.f,200.f }, L"Start Game", this, 64);
	AddChild(m_StartGameButton);

	m_OptionsButton = new UiButton<MainMenu, &MainMenu::GoToOptions>(fontPath, soundForwardPath, { 450.f,350.f }, L"Options", this, 64);
	AddChild(m_OptionsButton);

	m_QuitGameButton = new UiButton<MainMenu, &MainMenu::QuitGame>(fontPath, soundBackwardPath, { 425.f,500.f }, L"Quit Game", this, 64);
	AddChild(m_QuitGameButton);

	//Camera
	auto pObj = new GameObject();
	auto const gameContext = GetGameContext();
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

void MainMenu::GoToOptions()
{
	SceneManager::GetInstance()->SetActiveGameScene(L"Options menu");
}

