#include "stdafx.h"
#include "TestScene.h"

#include"Prefabs/BonfirePrefab.h"

TestScene::TestScene() :GameScene(L"TestScene")
{
}

TestScene::~TestScene()
{
}

void TestScene::Initialize()
{
	AddChild(new BonfirePrefab({0,0,0}));
}

void TestScene::Update()
{
}

void TestScene::Draw()
{
}
