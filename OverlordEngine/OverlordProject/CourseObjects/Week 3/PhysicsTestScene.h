#pragma once
#include "GameScene.h"

class PhysicsTestScene final : public GameScene
{
public:
	PhysicsTestScene();
	virtual ~PhysicsTestScene() = default;

	PhysicsTestScene(const PhysicsTestScene& other) = delete;
	PhysicsTestScene(PhysicsTestScene&& other) noexcept = delete;
	PhysicsTestScene& operator=(const PhysicsTestScene& other) = delete;
	PhysicsTestScene& operator=(PhysicsTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	void CreateSphere(physx::PxMaterial* pMat, CollisionGroupFlag group, float y);
	
};


