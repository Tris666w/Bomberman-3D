#pragma once
#include <GameObject.h>

class ModelComponent;
class RigidBodyComponent;
class ParticleEmitterComponent;

class BombPrefab final : public GameObject
{
public:
	BombPrefab();

	/**
	 * .Activates the bomb
	 *
	 * \param spawnPos the position where the bomb will be spawned at
	 * \param reach the reach of the bomb in each direction
	 */
	void Activate(const DirectX::XMFLOAT3& spawnPos,int reach);
	/**
	 * .
	 *
	 * \return m_IsActive
	 */
	bool GetIsActive()const { return m_IsActive; }

	/// @brief Immediately explode this bomb
	void Explode();
protected:
	void Initialize(const ::GameContext&) override;
	void PostInitialize(const GameContext&) override;
	void Draw(const ::GameContext&) override;
	void Update(const ::GameContext&) override;

private:
	bool m_IsActive = false;
	int m_Damage = 1;
	float const m_ExplodeTime = 3.f;
	float m_ExplodeTimer = 0.f;
	int m_ExplosionReach = -1;
	DirectX::XMFLOAT3 m_ExplodePos = {};
	bool m_IsExploding = false;

	ModelComponent* m_pModelComponent = nullptr;
	RigidBodyComponent* m_pRigidBody = nullptr;
	ParticleEmitterComponent* m_pSmokeEmitter = nullptr;

	FMOD::Channel* m_pChannel = nullptr;
	FMOD::Sound* m_pSound = nullptr;
	
	/// @brief Function to create explosions
	/// @param direction of the explosion
	/// @param reach  Distance of the explosion (in units)
	void CreateExplosion(DirectX::XMFLOAT3 direction, int reach);

	/// @brief spawns 1 explosion at the given pos
	/// @param spawnPos Spawn position of the explosion
	void CreateExplosion(DirectX::XMFLOAT3 spawnPos);

	/// @brief spawns 1 explosion at the given pos
	/// @param spawnPos Spawn position of the explosion
	void CreateExplosion(const physx::PxVec3& spawnPos);
};

