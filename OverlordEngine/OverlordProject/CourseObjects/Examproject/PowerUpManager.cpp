#include "stdafx.h"
#include "PowerUpManager.h"
#include "PowerUps.h"
#include "BombermanGameSettings.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "TransformComponent.h"

void PowerUpManager::AddPowerUp(PowerUpBase* pPowerUp)
{
	if (pPowerUp)
		m_pPowerUps.push_back(pPowerUp);
	else
		Logger::LogError(L" PowerUpManager::AddPowerUp, pPowerUp is nullptr!");
}

void PowerUpManager::TryPowerUpSpawn(DirectX::XMFLOAT3 spawnPos)
{
	if (randF(0.f, 1.f) < m_CurrentSpawnPercentage)
	{
		m_CurrentSpawnPercentage = BombermanGameSettings::GetInstance()->GetPowerUpSpawnPercent();
		float const spawnNumber = randF(0, 1.f);
		float total = BombermanGameSettings::GetInstance()->GetRadiusPowerUpPercentage();
		if (spawnNumber < total)
		{
			SpawnPowerUp<RadiusPowerUp>(spawnPos);
			return;
		}
		total += BombermanGameSettings::GetInstance()->GetLifePowerUpPercentage();
		if (spawnNumber < total)
		{
			SpawnPowerUp<LifePowerUp>(spawnPos);
			return;
		}
		total += BombermanGameSettings::GetInstance()->GetBombBeltPowerUpPercentage();
		if (spawnNumber < total)
		{
			SpawnPowerUp<BombBeltPowerup>(spawnPos);
			return;
		}
	}
	else
	{
		m_CurrentSpawnPercentage *= 1.5f;
	}
}

PowerUpManager::PowerUpManager()
	:m_pPowerUps{},
	m_CurrentSpawnPercentage(BombermanGameSettings::GetInstance()->GetPowerUpSpawnPercent())
{
}

template <class powerUpClass>
void PowerUpManager::SpawnPowerUp(DirectX::XMFLOAT3 spawnPos)
{
	if (std::is_base_of<PowerUpBase, powerUpClass>::value)
	{
		auto* pPowerUp = FindDisabledPowerUp<powerUpClass>();
		if (pPowerUp)
		{
			pPowerUp->SetIsEnabled(true);
			pPowerUp->GetTransform()->Translate(spawnPos);
		}
		else
		{
			Logger::LogInfo(L"Created new");
			CreateAndTranslate<powerUpClass>(spawnPos);
		}
	}
}

template <class powerUpClass>
PowerUpBase* PowerUpManager::FindDisabledPowerUp()
{
	Logger::LogWarning(L"PowerUpManager::FindDisabledPowerUp(), template power up type is not implemented!");

}

template<>
PowerUpBase* PowerUpManager::FindDisabledPowerUp<RadiusPowerUp>()
{
	//Check if the power up is of the correct type and if it isn't active
	auto it = std::find_if(m_pPowerUps.begin(), m_pPowerUps.end(), [](PowerUpBase* pPowerUp)
		{
			if (dynamic_cast<RadiusPowerUp*>(pPowerUp) && !pPowerUp->GetIsEnabled())
				return true;
			else
				return false;
		});
	if (it != m_pPowerUps.end())
	{
		return *it;
	}
	else
		return nullptr;
}
template<>
PowerUpBase* PowerUpManager::FindDisabledPowerUp<LifePowerUp>()
{
	//Check if the power up is of the correct type and if it isn't active
	auto it = std::find_if(m_pPowerUps.begin(), m_pPowerUps.end(), [](PowerUpBase* pPowerUp)
		{
			if (dynamic_cast<LifePowerUp*>(pPowerUp) && !pPowerUp->GetIsEnabled())
				return true;
			else
				return false;
		});
	if (it != m_pPowerUps.end())
	{
		return *it;
	}
	else
		return nullptr;
}
template<>
PowerUpBase* PowerUpManager::FindDisabledPowerUp<BombBeltPowerup>()
{
	//Check if the power up is of the correct type and if it isn't active
	auto it = std::find_if(m_pPowerUps.begin(), m_pPowerUps.end(), [](PowerUpBase* pPowerUp)
		{
			if (dynamic_cast<BombBeltPowerup*>(pPowerUp) && !pPowerUp->GetIsEnabled())
				return true;
			else
				return false;
		});
	if (it != m_pPowerUps.end())
	{
		return *it;
	}
	else
		return nullptr;
}



template <class powerUpClass>
void PowerUpManager::CreateAndTranslate(DirectX::XMFLOAT3 spawnPos)
{
	if (std::is_base_of<PowerUpBase, powerUpClass>::value)
	{
		auto pPowerUp = new powerUpClass();
		SceneManager::GetInstance()->GetActiveScene()->AddChild(pPowerUp);
		AddPowerUp(pPowerUp);
		pPowerUp->SetIsEnabled(true);
		pPowerUp->GetTransform()->Translate(spawnPos);
	}
}
