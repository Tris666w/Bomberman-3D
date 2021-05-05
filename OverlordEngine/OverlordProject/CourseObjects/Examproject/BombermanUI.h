#pragma once
#include <GameObject.h>
class SpriteComponent;
class SpriteFont;
class BombermanCharPrefab;
class BombermanUi final:public GameObject
{
public:
	BombermanUi(BombermanCharPrefab* pPlayer, DirectX::XMFLOAT2, bool drawUnder = false);
	~BombermanUi() override;
protected:
	void Initialize(const GameContext&) override;
	void PostInitialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void PostDraw(const GameContext&) override;
	void Update(const GameContext&) override;

private:
	SpriteFont* m_pFont;
	SpriteComponent* m_pSprite;
	BombermanCharPrefab* m_pPlayer;
	DirectX::XMFLOAT2 m_DrawPos;
	bool m_DrawUnder;
};

