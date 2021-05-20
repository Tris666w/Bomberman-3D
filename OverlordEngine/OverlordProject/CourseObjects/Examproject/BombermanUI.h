#pragma once
#include <GameObject.h>
class SpriteComponent;
class BombermanCharPrefab;
class BombermanUi final:public GameObject
{
public:
	BombermanUi(const std::wstring& assetPath, BombermanCharPrefab* pPlayer, DirectX::XMFLOAT2 drawPos);
	~BombermanUi() override;

protected:
	void Initialize(const GameContext&) override;
	void PostInitialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void PostDraw(const GameContext&) override;
	void Update(const GameContext&) override;

private:
	std::wstring m_TexturePath;
	SpriteComponent* m_pSprite;
	BombermanCharPrefab* m_pPlayer;
	DirectX::XMFLOAT2 m_DrawPos;
	bool m_PlayerHasDied;
};

