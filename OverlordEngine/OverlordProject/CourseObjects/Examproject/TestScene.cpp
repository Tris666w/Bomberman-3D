#include "stdafx.h"
#include "TestScene.h"

#include "Materials/Post/PostColorGrading.h"
#include"Prefabs/BonfirePrefab.h"

TestScene::TestScene() :GameScene(L"TestScene")
{
}

TestScene::~TestScene()
{
}

void TestScene::Initialize()
{
	AddChild(new BonfirePrefab({ -10,0,0 }));
	auto* pp = new PostColorGrading();
	pp->SetContribution(0.4f);
	pp->SetLUT(L"Resources/Textures/test_lut.png");
	AddPostProcessingEffect(pp);
}

void TestScene::Update()
{
}

void TestScene::Draw()
{
}
