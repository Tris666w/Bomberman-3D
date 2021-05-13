#pragma once
#include <GameObject.h>
class StumpPrefab final :public GameObject
{
public:
	StumpPrefab(int matId);
	~StumpPrefab();

	void Break();

protected:
	void Initialize(const GameContext&) override;
	void Draw(const GameContext&) override;
	void Update(const GameContext&) override;
	void PostInitialize(const GameContext&) override;
private:
	int m_MatId;

};

