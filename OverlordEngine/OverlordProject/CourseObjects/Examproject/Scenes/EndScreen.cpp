#include "stdafx.h"
#include "EndScreen.h"

#include "../Text.h"
#include "GameObject.h"
#include "OverlordGame.h"
#include "ModelComponent.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "../BombermanGameSettings.h"
#include "../../../Materials/DiffuseMaterial.h"

EndScreen::EndScreen(size_t winnerIndex) :GameScene(L"End screen"),
m_WinnerIndex(winnerIndex)
{
}

void EndScreen::Initialize()
{
	std::wstring const fontPath = L"Resources/SpriteFonts/BombermanBig.fnt";
	std::wstring const soundForwardPath = L"Resources/Sounds/ForwardSelect.wav";
	auto& window = OverlordGame::GetGameSettings().Window;
	int const charW = 48;
	int const charH = 64;

	
	std::wstring text = L"Player " + std::to_wstring(m_WinnerIndex) + L" has won!";
	DirectX::XMFLOAT2 drawPos = DirectX::XMFLOAT2(window.Width / 2.f - (text.size() * 0.5f * charW),
		window.Height / 2.f - charH);

	auto* pText = new Text(text, drawPos, BombermanGameSettings::text_color, fontPath);
	AddChild(pText);

	text  = L"Contrinue";
	drawPos.x = window.Width / 2.f - (text.size() * 0.5f * charW);
	drawPos.y += 2 * charH;

	m_pContinueButton = new UiButton<EndScreen, &EndScreen::ContinueToMenu>(fontPath, soundForwardPath, drawPos,
		L"Continue", this, charH);
	AddChild(m_pContinueButton);

	CreateCamera();
	CreateBackground();
	InitializeMusic();
}

void EndScreen::Update()
{
	m_pContinueButton->CheckForActivation(GetGameContext());
}

void EndScreen::Draw()
{
}

void EndScreen::SceneActivated()
{
	GetGameContext().pInput->Initialize();
	if (m_pChannel)
	{
		m_pChannel->setPaused(false);
		m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetMusicVolume());
	}
}

void EndScreen::SceneDeactivated()
{
	if (m_pChannel)
	{
		m_pChannel->setPaused(true);
	}
	SceneManager::GetInstance()->RemoveGameScene(this);
}

void EndScreen::ContinueToMenu(void*)
{
	auto sceneManager = SceneManager::GetInstance();
	auto bombermanScene = sceneManager->GetScene(L"BombermanScene");
	if (bombermanScene)
	{
		sceneManager->RemoveGameScene(bombermanScene);
	}
	GetGameContext().pInput->Initialize();
	sceneManager->SetActiveGameScene(L"Main menu");
}

void EndScreen::CreateCamera()
{
	auto pObj = new GameObject();
	auto& gameContext = GetGameContext();
	auto const input = InputAction(2, InputTriggerState::Pressed, 'R');
	gameContext.pInput->AddInputAction(input);

	auto const pCam = new CameraComponent();
	pObj->AddComponent(pCam);
	pObj->GetTransform()->Translate(29.f, 20.f, -39.f);
	SetActiveCamera(pCam);
	AddChild(pObj);
}

void EndScreen::CreateBackground()
{
	auto* const pObj = new GameObject();
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

void EndScreen::InitializeMusic()
{
	FMOD::Sound* pSound;
	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/EndMusic.mp3", FMOD_DEFAULT, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_NORMAL);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	
	SoundManager::GetInstance()->GetSystem()->playSound(pSound, nullptr, false, &m_pChannel);
	m_pChannel->setVolume(BombermanGameSettings::GetInstance()->GetMusicVolume());
	m_pChannel->setPaused(true);
}
