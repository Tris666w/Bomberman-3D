#pragma once
#include <GameObject.h>
class SpriteComponent;
class BombermanCharPrefab;
class BombermanUi final :public GameObject
{
public:
	BombermanUi(const std::wstring& assetPath, const std::wstring& bombTexture, const std::wstring& heartTexture, BombermanCharPrefab* pPlayer, DirectX::XMFLOAT2 drawPos);
	~BombermanUi() override;

protected:
	void Initialize(const GameContext&) override;
	void PostInitialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void PostDraw(const GameContext&) override;
	void Update(const GameContext&) override;

private:
	std::wstring m_TexturePath;
	std::wstring m_BombTexturePath;
	std::wstring m_HeartTexturePath;

	SpriteComponent* m_pSprite;
	std::vector<SpriteComponent*>m_pBombSprites;
	std::vector<SpriteComponent*>m_pHeartSprites;
	BombermanCharPrefab* m_pPlayer;
	DirectX::XMFLOAT2 m_DrawPos;
	bool m_PlayerHasDied;
};

