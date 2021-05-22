#pragma once
#include <Singleton.h>
class BombPrefab;
class BombManager :public Singleton<BombManager>
{
public:
	~BombManager() = default;
	void AddBomb(BombPrefab* pBomb);
	BombPrefab* GetBomb();
private:
	friend class Singleton<BombManager>;

	BombManager();

	std::vector<BombPrefab*> m_pBombs = {};
};

