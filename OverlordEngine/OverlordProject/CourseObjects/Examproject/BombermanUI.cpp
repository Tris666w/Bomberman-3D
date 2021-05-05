#include "stdafx.h"
#include "BombermanUI.h"

#include "ContentManager.h"
#include "OverlordGame.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "Prefabs/BombermanCharPrefab.h"
#include "SpriteFont.h"
#include "TextRenderer.h"

BombermanUi::BombermanUi(BombermanCharPrefab* pPlayer, DirectX::XMFLOAT2 drawPos, bool drawUnder)
	:m_pFont(nullptr),
	m_pSprite(nullptr),
	m_pPlayer(pPlayer),
	m_DrawPos(drawPos),
	m_DrawUnder(drawUnder)
{
}

BombermanUi::~BombermanUi()
{
}

void BombermanUi::Initialize(const GameContext&)
{
	m_pSprite = new SpriteComponent(L"./Resources/Textures/BombSprite.png", DirectX::XMFLOAT2(0.f, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1.f));
	AddComponent(m_pSprite);
	GetTransform()->Scale(.25f, 0.25f, 0.25f);
	if (m_DrawUnder)
		GetTransform()->Translate(m_DrawPos.x + 25.f, m_DrawPos.y - 80.f, 0.f);
	else
		GetTransform()->Translate(m_DrawPos.x + 25.f, m_DrawPos.y + 30.f, 0.f);

	
	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Bomberman.fnt");
}

void BombermanUi::PostInitialize(const GameContext&)
{
}

void BombermanUi::Draw(const GameContext&)
{
	if (m_pFont->GetFontName() != L"")
	{
		auto const pWindow =  OverlordGame::GetGameSettings().Window;
		switch (m_pPlayer->GetPlayerIndex())
		{
		case GamepadIndex::PlayerOne:
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, L"Player 1!", m_DrawPos, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Wheat));
			}
			break;
		case GamepadIndex::PlayerTwo:
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, L"Player 2!", m_DrawPos, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Wheat));
			}
			break;
		case GamepadIndex::PlayerThree:
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, L"Player 3!", m_DrawPos, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Wheat));
			}
			break;
		case GamepadIndex::PlayerFour:
			{
				TextRenderer::GetInstance()->DrawText(m_pFont, L"Player 4!", m_DrawPos, static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Wheat));
			}
			break;
		}
		
	}
}

void BombermanUi::PostDraw(const GameContext&)
{
}

void BombermanUi::Update(const GameContext&)
{
	if (m_pPlayer->GetCanSpawnBomb())
		m_pSprite->SetColor({1.f,1.f,1.f,1.f});
	else
		m_pSprite->SetColor({0,0,0,0});

}
