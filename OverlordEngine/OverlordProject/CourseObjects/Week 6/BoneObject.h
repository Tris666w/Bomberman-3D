#pragma once
#include "GameObject.h"
class BoneObject final : public GameObject
{
public:
	BoneObject(int boneId, int materialId, float length = 5.0f); ~BoneObject() = default;
		
	BoneObject(const BoneObject& other) = delete;
	BoneObject(BoneObject&& other) noexcept = delete;
	BoneObject& operator=(const BoneObject& other) = delete;
	BoneObject& operator=(BoneObject&& other) noexcept = delete;
		
	void AddBone(BoneObject* pBone);
protected:
	virtual void Initialize(const GameContext& gameContext);
private:
	float m_Length;
	int m_BoneId;
	int m_MaterialId;
};

