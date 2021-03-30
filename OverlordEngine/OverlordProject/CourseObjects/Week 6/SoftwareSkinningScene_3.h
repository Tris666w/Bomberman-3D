#pragma once
#include <GameScene.h>
#include <vector>
class MeshDrawComponent;
struct SkinnedVertex;
class BoneObject;

class SoftwareSkinningScene_3 final:  public GameScene
{
public:
	SoftwareSkinningScene_3();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	BoneObject *m_pBone0, *m_pBone1;
	float m_BoneRotation;
	int m_RotationSign;
	MeshDrawComponent* m_pMeshDrawer;
	std::vector<SkinnedVertex> m_SkinnedVertices;
	void CreateMesh(float length);
};

