#pragma once
#include <GameObject.h>
class ParticleEmitterComponent;

class Explosion final: public GameObject
{
public:
	Explosion();
	~Explosion() override;
protected:
	void Initialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void Update(const GameContext&) override;

private:
	ParticleEmitterComponent* m_pParticleEmitter = nullptr;
};

