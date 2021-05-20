#include "stdafx.h"
#include "InGameMenu.h"

#include "../Text.h"
#include "SceneManager.h"
#include "OverlordGame.h"
#include "BombermanScene.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "../../../Materials/DiffuseMaterial.h"
InGameMenu::InGameMenu() :GameScene(L"In-game menu"),
m_MainMenuButton(nullptr),
m_RestartButton(nullptr),
m_QuitGameButton(nullptr)
{
}

InGameMenu::~InGameMenu()
{
}

void InGameMenu::Initialize()
{
	std::wstring const fontPath = L"Resources/SpriteFonts/BombermanBig.fnt";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";
	auto& window = OverlordGame::GetGameSettings().Window;

	auto drawPos = DirectX::XMFLOAT2(window.Width / 20.f, window.Height / 5.f);
	m_ContinueButton = new UiButton<InGameMenu, &InGameMenu::ContinueGame>(fontPath, soundForwardPath, { drawPos }, L"Resume", this, 64);
	AddChild(m_ContinueButton);

	drawPos.y += window.Height / 5.f;
	m_MainMenuButton = new UiButton<InGameMenu, &InGameMenu::LoadMainMenu>(fontPath, soundForwardPath, { drawPos }, L"Main menu", this, 64);
	AddChild(m_MainMenuButton);

	drawPos.y += window.Height / 5.f;
	m_RestartButton = new UiButton<InGameMenu, &InGameMenu::RestartGame>(fontPath, soundForwardPath, { drawPos }, L"Restart", this, 64);
	AddChild(m_RestartButton);

	drawPos.y += window.Height / 5.f;
	m_QuitGameButton = new UiButton<InGameMenu, &InGameMenu::QuitGame>(fontPath, soundForwardPath, { drawPos }, L"Quit Game", this, 64);
	AddChild(m_QuitGameButton);

	InitSoundButtons();

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
}

void InGameMenu::Update()
{
	auto& gameContext = GetGameContext();
	m_ContinueButton->CheckForActivation(gameContext);
	m_MainMenuButton->CheckForActivation(gameContext);
	m_RestartButton->CheckForActivation(gameContext);
	m_QuitGameButton->CheckForActivation(gameContext);

	float volume = BombermanGameSettings::GetInstance()->GetSoundVolume() - 0.1f;
	m_SoundVolumeMinusButton->CheckForActivation(GetGameContext(), &volume);
	volume += 0.2f;
	m_SoundVolumePlusButton->CheckForActivation(GetGameContext(), &volume);

	volume = BombermanGameSettings::GetInstance()->GetMusicVolume() - 0.1f;
	m_MusicVolumeMinusButton->CheckForActivation(GetGameContext(), &volume);
	volume += 0.2f;
	m_MusicVolumePlusButton->CheckForActivation(GetGameContext(), &volume);

}

void InGameMenu::Draw()
{
}

void InGameMenu::ContinueGame(void*)
{
	SceneManager::GetInstance()->SetActiveGameScene(L"BombermanScene");
}

void InGameMenu::LoadMainMenu(void*)
{
	SceneManager::GetInstance()->SetActiveGameScene(L"Main menu");
}

void InGameMenu::QuitGame(void*)
{
	PostQuitMessage(0);
}

void InGameMenu::ChangeSoundVolume(void* pData)
{
	auto pFloat = static_cast<float*>(pData);
	float volume = *pFloat;
	Clamp(volume, 1.f, 0.f);
	BombermanGameSettings::GetInstance()->SetSoundVolume(volume);
	m_pSoundText->SetText(std::to_wstring(static_cast<int>(volume * 10)));
}

void InGameMenu::ChangeMusicVolume(void* pData)
{
	auto pFloat = static_cast<float*>(pData);
	float volume = *pFloat;
	Clamp(volume, 1.f, 0.f);
	BombermanGameSettings::GetInstance()->SetMusicVolume(volume);
	m_pMusicText->SetText(std::to_wstring(static_cast<int>(volume * 10)));
}

void InGameMenu::InitSoundButtons()
{
	std::wstring const fontPathSmall = L"Resources/SpriteFonts/BombermanMedium.fnt";

	std::wstring const fontPathBig = L"Resources/SpriteFonts/BombermanBig.fnt";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";
	std::wstring const soundBackwardPath = L"Resources/Sounds/BackwardSelect.wav";

	int const charSize = 64;

	auto& window = OverlordGame::GetGameSettings().Window;

	DirectX::XMFLOAT2 drawPos = { window.Width / 2.f,window.Height / 6.f };

	auto* pText = new Text(L"Music", { drawPos }, BombermanGameSettings::text_color, fontPathSmall);
	AddChild(pText);

	drawPos.y += charSize;
	m_MusicVolumePlusButton = new UiButton<InGameMenu, &InGameMenu::ChangeMusicVolume>(fontPathBig, soundForwardPath, { drawPos }, L"+", this, charSize);
	AddChild(m_MusicVolumePlusButton);

	drawPos.x += charSize;
	m_pMusicText = new Text(std::to_wstring(static_cast<int>(BombermanGameSettings::GetInstance()->GetMusicVolume() * 10)), { drawPos }, BombermanGameSettings::text_color, fontPathBig);
	AddChild(m_pMusicText);

	drawPos.x += 1.5f * charSize;
	m_MusicVolumeMinusButton = new UiButton<InGameMenu, &InGameMenu::ChangeMusicVolume>(fontPathBig, soundBackwardPath, { drawPos }, L"-", this, charSize);
	AddChild(m_MusicVolumeMinusButton);

	drawPos = { window.Width / 2.f,4 * window.Height / 6.f };
	pText = new Text(L"Sound", { drawPos }, BombermanGameSettings::text_color, fontPathSmall);
	AddChild(pText);

	drawPos.y += charSize;
	m_SoundVolumePlusButton = new UiButton<InGameMenu, &InGameMenu::ChangeSoundVolume>(fontPathBig, soundForwardPath, { drawPos }, L"+", this, charSize);
	AddChild(m_SoundVolumePlusButton);

	drawPos.x += charSize;
	m_pSoundText = new Text(std::to_wstring(static_cast<int>(BombermanGameSettings::GetInstance()->GetSoundVolume() * 10)), { drawPos }, BombermanGameSettings::text_color, fontPathBig);
	AddChild(m_pSoundText);

	drawPos.x += 1.5f * charSize;
	m_SoundVolumeMinusButton = new UiButton<InGameMenu, &InGameMenu::ChangeSoundVolume>(fontPathBig, soundBackwardPath, { drawPos }, L"-", this, charSize);
	AddChild(m_SoundVolumeMinusButton);
}

void InGameMenu::RestartGame(void*)
{
	auto sceneManager = SceneManager::GetInstance();
	auto bombermanScene = sceneManager->GetScene(L"BombermanScene");
	if (bombermanScene)
	{
		sceneManager->RemoveGameScene(bombermanScene);
		sceneManager->AddGameScene(new BombermanScene());
		sceneManager->SetActiveGameScene(L"BombermanScene");
	}
}
