#pragma once
#include <GameObject.h>

class BombermanCharPrefab;
class ParticleEmitterComponent;
class PowerUpBase : public GameObject
{
public:
	virtual void ExecuteEffect(BombermanCharPrefab* pChar) = 0;
	void SetParticleEmitter (ParticleEmitterComponent* pEmitter);
	void SetSound(FMOD::Sound* sound);

	[[nodiscard]]const ParticleEmitterComponent* GetParticleEmitter()const{return m_pEmitter;}
	[[nodiscard]]const FMOD::Sound* GetSound()const{return m_pSound;}
	
	void SetIsEnabled(bool enabled);
	[[nodiscard]]bool GetIsEnabled()const {return m_IsEnabled;};

	void PlayEffectSound()const;
protected:
	/// @brief Initializes the base, ALWAYS CALL IN THE INITIALIZE() FUNCTION
	void Init();
private:
		ParticleEmitterComponent* m_pEmitter = nullptr;
	FMOD::Sound* m_pSound = nullptr;
	bool m_IsEnabled = false;
};

