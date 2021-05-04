#pragma once
#include <BaseComponent.h>
class DestroyableWallComponent : public BaseComponent
{
public:
	void DestroyObject()const;
protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
};

