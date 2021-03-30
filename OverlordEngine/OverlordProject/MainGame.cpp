
#include "stdafx.h"

#include "MainGame.h"
#include "GeneralStructs.h"
#include "SceneManager.h"
#include "PhysxProxy.h"
#include "DebugRenderer.h"

#define W5_B

#ifdef W4_A
#include "CourseObjects/Week 4/ModelTestScene.h"
#include "CourseObjects/Week 4/SpriteTestScene.h"
#include "CourseObjects/Week 4/UberScene.h"
#endif

#ifdef W4_B
#include "CourseObjects/Week 4/SpikeyScene.h"
#endif

#ifdef W5_A
#include "CourseObjects/Week 5/FontTestScene.h"
#endif

#ifdef W5_B
#include "CourseObjects/Week 5/PickScene.h"
#include "CourseObjects/Week 5/CharacterScene.h"
#endif

MainGame::MainGame(void)
{}

MainGame::~MainGame(void)
{}

//Game is preparing
void MainGame::OnGamePreparing(GameSettings& gameSettings)
{
	UNREFERENCED_PARAMETER(gameSettings);
	//Nothing to do atm.
}

void MainGame::Initialize()
{
#ifdef W4_A
	SceneManager::GetInstance()->AddGameScene(new ModelTestScene());
	SceneManager::GetInstance()->AddGameScene(new SpriteTestScene());
	SceneManager::GetInstance()->AddGameScene(new UberScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"SpriteTestScene");
#endif

#ifdef W4_B
	SceneManager::GetInstance()->AddGameScene(new SpikeyScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"SpikeyScene");
#endif

#ifdef W5_A
	SceneManager::GetInstance()->AddGameScene(new FontTestScene());
	SceneManager::GetInstance()->SetActiveGameScene(L"FontTestScene");
#endif

#ifdef W5_B
	SceneManager::GetInstance()->AddGameScene(new PickScene());
	SceneManager::GetInstance()->AddGameScene(new CharacterScene());
	
	SceneManager::GetInstance()->SetActiveGameScene(L"CharacterScene");
#endif
}

LRESULT MainGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
		case WM_KEYUP:
		{
			if ((lParam & 0x80000000) != 0x80000000)
				return -1;

			//NextScene
			if (wParam == VK_F3)
			{
				SceneManager::GetInstance()->NextScene();
				return 0;
			}
			//PreviousScene
			else if (wParam == VK_F2)
			{
				SceneManager::GetInstance()->PreviousScene();
				return 0;
			}
			else if (wParam == VK_F4)
			{
				DebugRenderer::ToggleDebugRenderer();
				return 0;
			}
			else if (wParam == VK_F6)
			{
				auto activeScene = SceneManager::GetInstance()->GetActiveScene();
				activeScene->GetPhysxProxy()->NextPhysXFrame();
			}
		}
	}

	return -1;
}
