#pragma once
#include <GameScene.h>
class BoneObject;

class SoftwareSkinningScene_1 final:  public GameScene
{
public:
	SoftwareSkinningScene_1();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	BoneObject *m_pBone0, *m_pBone1;
	float m_BoneRotation;
	int m_RotationSign;
};

