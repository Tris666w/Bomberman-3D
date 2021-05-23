#include "stdafx.h"
#include "ExplosionManager.h"
#include "Prefabs/Explosion.h"

void ExplosionManager::AddExplosion(Explosion* pExplosion)
{
	if (std::find(m_pExplosions.begin(), m_pExplosions.end(), pExplosion) != m_pExplosions.end())
		Logger::LogWarning(L"ExplosionManager::AddExplosion, Trying to add explosion that is already in the manager ");
	else
		m_pExplosions.push_back(pExplosion);
}

Explosion* ExplosionManager::GetExplosion()
{
	for (auto& pExplosion : m_pExplosions)
	{
		if (!pExplosion->GetIsEnabled())
			return pExplosion;
	}
	return nullptr;
}
ExplosionManager::ExplosionManager()
{
}
