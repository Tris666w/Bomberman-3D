#pragma once
#include <Singleton.h>

class PowerUpBase;
class PowerUpManager : public Singleton<PowerUpManager>
{
public:
	void AddPowerUp(PowerUpBase* pPowerUp);
	~PowerUpManager() = default;

	static int const amount_of_start_power_ups = 20;
	void TryPowerUpSpawn(DirectX::XMFLOAT3 spawnPos);
private:
	friend class Singleton<PowerUpManager>;
	PowerUpManager();

	std::vector<PowerUpBase*> m_pPowerUps{};

	float m_CurrentSpawnPercentage;


	template <class powerUpClass>
	void SpawnPowerUp(DirectX::XMFLOAT3 spawnPos);
	
	template <class powerUpClass>
	PowerUpBase* FindDisabledPowerUp();
	
	template <class powerUpClass>
	void CreateAndTranslate(DirectX::XMFLOAT3 spawnPos);
	
};



