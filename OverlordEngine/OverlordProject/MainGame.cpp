
#include "stdafx.h"

#include "MainGame.h"
#include "GeneralStructs.h"
#include "SceneManager.h"
#include "PhysxProxy.h"
#include "DebugRenderer.h"

#define W3

#ifdef W3
#include "CourseObjects/Week 3/MinionScene.h"
#include "CourseObjects/Week 3/PhysicsTestScene.h"
#include "CourseObjects/Week 3/TutorialScene.h"
#include "CourseObjects/Week 3/PongScene.h"
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
	
#ifdef W3
	SceneManager::GetInstance()->AddGameScene(new PongScene());
	SceneManager::GetInstance()->AddGameScene(new PhysicsTestScene());
	SceneManager::GetInstance()->AddGameScene(new MinionScene());
	SceneManager::GetInstance()->AddGameScene(new TutorialScene());
	
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
