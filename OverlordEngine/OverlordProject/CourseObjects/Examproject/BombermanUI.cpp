#include "stdafx.h"
#include "BombermanUI.h"

#include "ContentManager.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "Prefabs/BombermanCharPrefab.h"

BombermanUi::BombermanUi(const std::wstring& assetPath, const std::wstring& bombTexture, const std::wstring& heartTexture, BombermanCharPrefab* pPlayer, DirectX::XMFLOAT2 drawPos)
	:m_TexturePath(assetPath),
	m_BombTexturePath(bombTexture),
	m_HeartTexturePath(heartTexture),
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
	auto* pObj = new GameObject();
	m_pSprite = new SpriteComponent(m_TexturePath, DirectX::XMFLOAT2(0, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1.f));
	pObj->AddComponent(m_pSprite);
	AddChild(pObj);
	for (int i = 0; i < m_pPlayer->GetMaxAmountOfBombs(); ++i)
	{
		pObj = new GameObject();
		auto const pSprite = new SpriteComponent(m_BombTexturePath, DirectX::XMFLOAT2(0, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1.f));
		pObj->AddComponent(pSprite);
		m_pBombSprites.push_back(pSprite);
		AddChild(pObj);
	}

	for (int i = 0; i < m_pPlayer->GetMaxHealth(); ++i)
	{
		pObj = new GameObject();
		auto const pSprite = new SpriteComponent(m_HeartTexturePath, DirectX::XMFLOAT2(0, 0.f), DirectX::XMFLOAT4(1, 1, 1, 1.f));
		pObj->AddComponent(pSprite);
		m_pHeartSprites.push_back(pSprite);
		AddChild(pObj);
	}
}

void BombermanUi::PostInitialize(const GameContext&)
{
	GetTransform()->Translate(m_DrawPos.x, m_DrawPos.y, 0.f);

	float initialXOffset = m_pSprite->GetTextureDimensions().x;
	float xOffset = m_pBombSprites[0]->GetTextureDimensions().y*0.25f;
	for (size_t i = 0; i < m_pBombSprites.size(); ++i)
	{
		m_pBombSprites[i]->GetTransform()->Translate( initialXOffset + i * xOffset, 0, 0.f);
	}

	float yOffset = m_pBombSprites[0]->GetTextureDimensions().y;
	for (size_t i = 0; i < m_pHeartSprites.size(); ++i)
	{
		m_pHeartSprites[i]->GetTransform()->Translate(initialXOffset + i * xOffset, yOffset, 0.f);
	}

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
		m_pSprite->SetColor({ 0,0,0,0 });
	}

	int amountOfBombs = m_pPlayer->GetAmountOfBombs();
	for (int i = 0; i < amountOfBombs; ++i)
	{
		m_pBombSprites[i]->SetColor({ 1, 1, 1, 1 });
	}
	for (int i = 1; i <= static_cast<int>(m_pBombSprites.size()) - amountOfBombs; ++i)
	{
		m_pBombSprites[m_pBombSprites.size() - i]->SetColor({ 1, 1, 1, 0 });
	}

	int amountOfHearts = m_pPlayer->GetHealth();
	for (int i = 0; i < amountOfHearts; ++i)
	{
		m_pHeartSprites[i]->SetColor({ 1, 1, 1, 1 });
	}
	for (int i = 1; i <= static_cast<int>(m_pHeartSprites.size()) - amountOfHearts; ++i)
	{
		m_pHeartSprites[m_pHeartSprites.size() - i]->SetColor({ 1, 1, 1, 0 });
	}
}
