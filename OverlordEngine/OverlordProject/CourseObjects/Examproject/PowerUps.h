#pragma once
#include "PowerUpBase.h"
class RadiusPowerUp final : public PowerUpBase
{
public:
	RadiusPowerUp() = default;
	void ExecuteEffect(BombermanCharPrefab* pChar) override;

protected:
	void Initialize(const GameContext&) override;

	void Update(const GameContext&) override;

private:
	bool m_Activated = false;
	int m_BaseBombRange = 1;
	int m_RangeAdd = 2;
	BombermanCharPrefab* m_pAffectedChar = nullptr;

	float m_EffectTime = 7.5f;
	float m_Timer = 0.f;
};

class LifePowerUp final : public PowerUpBase
{
public:
	LifePowerUp();
	void ExecuteEffect(BombermanCharPrefab* pChar) override;
protected:
	void Initialize(const GameContext&) override;
};


class BombBeltPowerup final : public PowerUpBase
{
public:
	BombBeltPowerup() = default;
	void ExecuteEffect(BombermanCharPrefab* pChar) override;
protected:
	void Initialize(const GameContext&) override;
};