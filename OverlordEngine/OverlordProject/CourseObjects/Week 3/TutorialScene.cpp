#include "stdafx.h"
#include "TutorialScene.h"

#include "TorusPrefab.h"
#include "TransformComponent.h"

TutorialScene::TutorialScene(): GameScene(L"TutorialScene")
{
}

void TutorialScene::Initialize()
{
	Logger::LogInfo(L"Welcome to the tutorial!");
	m_pTorus1 = new TorusPrefab(1, 32, 0.4f, 12, DirectX::XMFLOAT4{1,0,0,1});
	AddChild(m_pTorus1);

	m_pTorus2 = new TorusPrefab(2, 32, 0.4f, 12, DirectX::XMFLOAT4{1,1,0,1});
	AddChild(m_pTorus2);

	m_pTorus3 = new TorusPrefab(3, 32, 0.4f, 12, DirectX::XMFLOAT4{0,1,1,1});
	AddChild(m_pTorus3);
}

void TutorialScene::Update()
{
	const auto& gameContext = GetGameContext();
	float const deltaTime = gameContext.pGameTime->GetElapsed();

	m_Angle1 += deltaTime * 30;
	m_pTorus1->GetTransform()->Rotate(0,m_Angle1,  0);

	m_Angle2 -= deltaTime * 30;
	m_pTorus2->GetTransform()->Rotate(0,m_Angle2,  m_Angle1);

	m_Angle3 += deltaTime * 60;
	m_pTorus3->GetTransform()->Rotate(m_Angle1,m_Angle3,  m_Angle2);
}

void TutorialScene::Draw()
{
}
