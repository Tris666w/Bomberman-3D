#pragma once
#include <GameObject.h>

class ModelComponent;
class RigidBodyComponent;

class BombPrefab final :  public GameObject
{
public:
	BombPrefab();
	void Activate(const DirectX::XMFLOAT3&);
	bool GetIsActive()const {return m_IsActive;}
protected:
	void Initialize(const ::GameContext&) override;
	void Draw(const ::GameContext&) override;
	void Update(const ::GameContext&) override;

private:
	void Explode();

	bool m_IsActive =false;

	float const m_ExplodeTime = 2.f;
	float m_ExplodeTimer = 0.f;

	ModelComponent* m_pModelComponent = nullptr;
	RigidBodyComponent* m_pRigigdBody = nullptr;
};

