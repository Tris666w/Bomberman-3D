#include "stdafx.h"
#include "MainMenu.h"
#include "..\UiButton.h"

template <auto* F>
struct Wrapper {};

template <class Ret, class... Args, auto (*F)(Args...) -> Ret>
struct Wrapper<F>
{
    auto operator()(Args... args)
    {
        return F(args...);
    }
};

MainMenu::MainMenu() :GameScene(L"Main menu")
{
}

void MainMenu::Print()
{
	Logger::LogInfo(L"REEEEEE");
}

void MainMenu::Initialize()
{
	std::wstring const fontPath = L"Resources/SpriteFonts/BombermanBig.fnt";
	pButton =new UiButton<MainMenu,&MainMenu::Print>(fontPath,{0.f,0.f},L"Start Game",this,64);
	AddChild(pButton);
}


void MainMenu::Update()
{
	pButton->CheckForActivation(GetGameContext());
}

void MainMenu::Draw()
{
}

