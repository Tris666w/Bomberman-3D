#pragma once
#include "BaseComponent.h"
class GameObject;

class LinkGameObjectPosComponent final :public BaseComponent
{
public:
	LinkGameObjectPosComponent(GameObject* pToFollow);
protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;

private:
	GameObject* m_pToFollowGameObject = nullptr;
};

