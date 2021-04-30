#pragma once
#include <Singleton.h>
class BombPrefab;
class BombManager :public Singleton<BombManager>
{
public:
	~BombManager();
	void AddBomb(BombPrefab* pBomb);
	BombPrefab* GetBomb();
private:
	friend class Singleton<BombManager>;

	BombManager() = default;
	
	std::vector<BombPrefab*> m_pBombs = {};
};

