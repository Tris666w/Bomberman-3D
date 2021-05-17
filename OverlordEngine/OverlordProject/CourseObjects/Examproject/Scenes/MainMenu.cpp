#include "stdafx.h"
#include "MainMenu.h"

#include "BombermanScene.h"
#include "SceneManager.h"
#include "..\UiButton.h"

MainMenu::MainMenu() :GameScene(L"Main menu"),
	m_StartGameButton(nullptr),
	m_QuitGameButton(nullptr)

{
}

void MainMenu::Print()
{
	Logger::LogInfo(L"Fuck dit man");
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

	m_StartGameButton = new UiButton<MainMenu,&MainMenu::StartGame>(fontPath,{0.f,0.f},L"Start Game",this,64);
	AddChild(m_StartGameButton);

	m_QuitGameButton = new UiButton<MainMenu,&MainMenu::QuitGame>(fontPath,{600.f,0.f},L"Quit Game",this,64);
	AddChild(m_QuitGameButton);
}


void MainMenu::Update()
{
	m_StartGameButton->CheckForActivation(GetGameContext());
	m_QuitGameButton->CheckForActivation(GetGameContext());
	
}

void MainMenu::Draw()
{
}

