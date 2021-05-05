#include "stdafx.h"
#include "BombManager.h"
#include "Prefabs/BombPrefab.h"

BombManager::~BombManager()
{
}

void BombManager::AddBomb(BombPrefab* pBomb)
{
	if (std::find(m_pBombs.begin(),m_pBombs.end(),pBomb) != m_pBombs.end())
		Logger::LogWarning(L"BombManager::AddBomb, Trying to add bomb that is already in the manager ");
	else
		m_pBombs.push_back(pBomb);
}

BombPrefab* BombManager::GetBomb()
{
	for (auto& pBomb : m_pBombs)
	{
		if (!pBomb->GetIsActive())
			return pBomb;
	}
	return nullptr;
}