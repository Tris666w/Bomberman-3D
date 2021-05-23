#pragma once
#include <Singleton.h>
class Explosion;
class ExplosionManager :public Singleton<ExplosionManager>
{
public:
	~ExplosionManager() = default;
	void AddExplosion(Explosion* pExplosion);
	Explosion* GetExplosion();
private:
	friend class Singleton<ExplosionManager>;
	ExplosionManager();

	std::vector<Explosion*> m_pExplosions = {};
};

