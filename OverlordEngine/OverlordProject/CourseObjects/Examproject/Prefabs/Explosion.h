#pragma once
#include <GameObject.h>
class ParticleEmitterComponent;

class Explosion final : public GameObject
{
public:
	Explosion();
	~Explosion() override;

	[[nodiscard]] bool GetIsEnabled()const;
	void SetIsEnabled(bool enabled);
protected:
	void Initialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void Update(const GameContext&) override;

private:
	ParticleEmitterComponent* m_pParticleEmitter = nullptr;
	bool m_IsEnabled = false;

	float m_Timer = 0.f;
	float const m_ExplodeTime = 0.5f;
};

