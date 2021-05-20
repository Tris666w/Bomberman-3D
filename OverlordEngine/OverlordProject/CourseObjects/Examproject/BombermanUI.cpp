#include "stdafx.h"
#include "BombermanUI.h"

#include "ContentManager.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "Prefabs/BombermanCharPrefab.h"

BombermanUi::BombermanUi(const std::wstring& assetPath,BombermanCharPrefab* pPlayer, DirectX::XMFLOAT2 drawPos)
	:m_TexturePath(assetPath),
	m_pSprite(nullptr),
	m_pPlayer(pPlayer),
	m_DrawPos(drawPos),
	m_PlayerHasDied(false)
{
}

BombermanUi::~BombermanUi()
{
}

void BombermanUi::Initialize(const GameContext&)
{
	m_pSprite = new SpriteComponent(m_TexturePath, DirectX::XMFLOAT2(0.f, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1.f));
	AddComponent(m_pSprite);
}

void BombermanUi::PostInitialize(const GameContext&)
{
	auto const drawY = m_DrawPos.y-m_pSprite->GetTextureDimensions().y*GetTransform()->GetScale().y;
	GetTransform()->Translate(m_DrawPos.x,drawY,0.f);
	
}

void BombermanUi::Draw(const GameContext&)
{
	
}

void BombermanUi::PostDraw(const GameContext&)
{
}

void BombermanUi::Update(const GameContext&)
{
	if (m_PlayerHasDied)
	{
		return;
	}
	if (m_pPlayer->GetIsDead())
	{
		m_PlayerHasDied = true;
		m_pSprite->SetColor({0,0,0,0});
	}
}
