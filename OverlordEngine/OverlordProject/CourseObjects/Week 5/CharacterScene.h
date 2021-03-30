#pragma once
#include <GameScene.h>
class CharacterScene final: public GameScene
{
public:
	CharacterScene();
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	
};

