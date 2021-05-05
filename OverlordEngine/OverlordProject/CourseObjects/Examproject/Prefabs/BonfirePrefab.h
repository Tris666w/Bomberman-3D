#pragma once
#include <GameObject.h>
class BonfirePrefab final:  public GameObject
{
public:
	explicit BonfirePrefab(DirectX::XMFLOAT3 spawnPos);
protected:
	void Initialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void Update(const GameContext&) override;

private:
	
};

