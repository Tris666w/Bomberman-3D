#include "stdafx.h"
#include "PreGameMenu.h"

#include "BombermanScene.h"
#include "GameObject.h"
#include "ModelComponent.h"
#include "OverlordGame.h"
#include "SceneManager.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "../Text.h"
#include "../../../Materials/DiffuseMaterial.h"

PreGameMenu::PreGameMenu() :GameScene(L"Pre game menu")
{
}

PreGameMenu::~PreGameMenu()
{
}

void PreGameMenu::Initialize()
{
	std::wstring const fontPathSmall = L"Resources/SpriteFonts/BombermanSmall.fnt";
	std::wstring const fontPathMedium = L"Resources/SpriteFonts/BombermanMedium.fnt";
	std::wstring const fontPathBig = L"Resources/SpriteFonts/BombermanBig.fnt";
	std::wstring const soundBackwardPath = L"Resources/Sounds/BackwardSelect.wav";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";

	int const charSizeSmall = 32;
	int const charSizeMedium = 48;
	int const charSizeBig = 64;

	auto& window = OverlordGame::GetGameSettings().Window;

	m_StartGameButton = new UiButton<PreGameMenu, &PreGameMenu::BeginGame>(fontPathBig, soundForwardPath, { 375.f,100.f }, L"Start Game", this, charSizeBig);
	AddChild(m_StartGameButton);

	m_MainMenuButton = new UiButton<PreGameMenu, &PreGameMenu::LoadMainMenu>(fontPathBig, soundBackwardPath, { 375.f,600.f }, L"Main menu", this, charSizeBig);
	AddChild(m_MainMenuButton);


	float const buttonY = 320.f;
	float x1 = 180.f;
	float x2 = 245;
	float x3 = 325;

	//For changing number of players
	auto* pText = new Text(L"Nr of players", { x1 - 5 * charSizeBig / 2,buttonY - 64 }, BombermanGameSettings::text_color, fontPathMedium);
	AddChild(pText);
	m_PlayerAddPlusButton = new UiButton<PreGameMenu, &PreGameMenu::SetAmountOfPLayers>(fontPathSmall, soundForwardPath, { x1,buttonY }, L"+", this, charSizeSmall);
	AddChild(m_PlayerAddPlusButton);
	m_pAmountOfPlayersText = new Text(std::to_wstring(BombermanGameSettings::GetInstance()->GetAmountOfPlayers()), { x2,buttonY }, BombermanGameSettings::text_color, fontPathSmall);
	AddChild(m_pAmountOfPlayersText);
	m_PlayerAddMinusButton = new UiButton<PreGameMenu, &PreGameMenu::SetAmountOfPLayers>(fontPathSmall, soundBackwardPath, { x3,buttonY }, L"-", this, charSizeSmall);
	AddChild(m_PlayerAddMinusButton);

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

	//
	auto drawPos = DirectX::XMFLOAT2(window.Width * 0.5f, buttonY - 64);

	std::wstring ControlsText = L"Change controls";
	pText = new Text(ControlsText, { drawPos }, BombermanGameSettings::text_color, fontPathMedium);
	AddChild(pText);

	drawPos.y += 64;

	std::wstring const playerText = L"Player:";
	pText = new Text(playerText, { drawPos }, BombermanGameSettings::text_color, fontPathSmall);
	AddChild(pText);

	drawPos.x += charSizeSmall * (playerText.size() - 1);
	m_pControllerChoosePlayerMinusButton = new UiButton<PreGameMenu, &PreGameMenu::CycleControllerChoosePlayer>(fontPathSmall, soundForwardPath, { drawPos }, L"-", this, charSizeSmall);
	AddChild(m_pControllerChoosePlayerMinusButton);

	drawPos.x += charSizeSmall * 1.f;
	m_pPlayerControlIndexText = new Text(std::to_wstring(m_CurrentPlayerIndex), { drawPos }, BombermanGameSettings::text_color, fontPathSmall);
	AddChild(m_pPlayerControlIndexText);

	drawPos.x += charSizeSmall * 1.f;
	m_pControllerChoosePlayerPlusButton = new UiButton<PreGameMenu, &PreGameMenu::CycleControllerChoosePlayer>(fontPathSmall, soundForwardPath, { drawPos }, L"+", this, charSizeSmall);
	AddChild(m_pControllerChoosePlayerPlusButton);

	pObj = new GameObject();
	m_pSchemeSprite = new SpriteComponent(L"Resources/Textures/InputSchemes/KeyboardScheme1.png");
	pObj->AddComponent(m_pSchemeSprite);
	AddChild(pObj);
	pObj->GetTransform()->Translate(drawPos.x, drawPos.y, 0);

	drawPos.x = window.Width * 0.5f;
	drawPos.y += charSizeBig * 1.5f;
	m_CycleInputControls = new UiButton<PreGameMenu, &PreGameMenu::SetInputScheme>(fontPathMedium, soundForwardPath, { drawPos }, L"Cycle", this, charSizeMedium);
	AddChild(m_CycleInputControls);

	InitializeMusic();
}

void PreGameMenu::Update()
{
	auto gameContext = GetGameContext();
	m_StartGameButton->CheckForActivation(gameContext);
	m_MainMenuButton->CheckForActivation(gameContext);

	auto amountOfPlayers = BombermanGameSettings::GetInstance()->GetAmountOfPlayers() + 1;
	m_PlayerAddPlusButton->CheckForActivation(gameContext, &amountOfPlayers);

	amountOfPlayers -= 2;
	m_PlayerAddMinusButton->CheckForActivation(gameContext, &amountOfPlayers);

	auto newIndex = m_CurrentPlayerIndex + 1;
	m_pControllerChoosePlayerPlusButton->CheckForActivation(gameContext, &newIndex);

	newIndex -= 2;
	m_pControllerChoosePlayerMinusButton->CheckForActivation(gameContext, &newIndex);

	newIndex = m_InputIndex + 1;
	m_CycleInputControls->CheckForActivation(gameContext,& newIndex);

	if (gameContext.pInput->IsActionTriggered(1000))
	{
		auto const mousePos = gameContext.pInput->GetMousePosition();
		Logger::LogInfo(L"Mouse coordinates are: x = " + std::to_wstring(mousePos.x) + L", y= " + std::to_wstring(mousePos.y));
	}
}

void PreGameMenu::Draw()
{
}

void PreGameMenu::SceneActivated()
{
	if (m_pChannel)
	{
		m_pChannel->setPaused(false);
		m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetMusicVolume());
	}

}

void PreGameMenu::SceneDeactivated()
{
	if (m_pChannel)
		m_pChannel->setPaused(true);
}

void PreGameMenu::LoadMainMenu(void*)
{
	SceneManager::GetInstance()->SetActiveGameScene(L"Main menu");
}

void PreGameMenu::BeginGame(void*)
{
	SceneManager::GetInstance()->AddGameScene(new BombermanScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"BombermanScene");
}

void PreGameMenu::SetAmountOfPLayers(void* pData)
{
	auto pInt = static_cast<int*>(pData);
	int amountOfPlayers = *pInt;
	Clamp(amountOfPlayers, BombermanGameSettings::max_amount_of_players, BombermanGameSettings::min_amount_of_players);
	BombermanGameSettings::GetInstance()->SetAmountOfPlayers(amountOfPlayers);
	m_pAmountOfPlayersText->SetText(std::to_wstring(amountOfPlayers));

	if (amountOfPlayers < m_CurrentPlayerIndex)
	{
		CycleControllerChoosePlayer(&amountOfPlayers);
	}

}

void PreGameMenu::CycleControllerChoosePlayer(void* pData)
{
	auto pInt = static_cast<int*>(pData);
	int newIndex = *pInt;
	Clamp(newIndex, BombermanGameSettings::GetInstance()->GetAmountOfPlayers(), 1);
	m_CurrentPlayerIndex = newIndex;
	m_pPlayerControlIndexText->SetText(std::to_wstring(newIndex));

	auto inputScheme = BombermanGameSettings::GetInstance()->GetPlayerInputScheme(m_CurrentPlayerIndex-1);
	SetInputScheme(&inputScheme);
}

void PreGameMenu::SetInputScheme(void* pData)
{
	m_InputIndex = *static_cast<int*>(pData);
	auto& schemeMap = BombermanGameSettings::GetInstance()->GetSchemeMap();
	if (m_InputIndex >= static_cast<int>(schemeMap.size()))
		m_InputIndex = 0;
	
	const auto& [controlVector, texturePath] = BombermanGameSettings::GetInstance()->GetSchemeMap().at(static_cast<InputScheme>(m_InputIndex));

	m_pSchemeSprite->SetTexture(texturePath);

	BombermanGameSettings::GetInstance()->SetPlayerInput(m_CurrentPlayerIndex - 1, static_cast<InputScheme>(m_InputIndex));
}

void PreGameMenu::InitializeMusic()
{
	FMOD::Sound* pSound;
	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/PreGame.wav", FMOD_DEFAULT, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_NORMAL);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	
	SoundManager::GetInstance()->GetSystem()->playSound(pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetMusicVolume());
	m_pChannel->setPaused(true);
}
