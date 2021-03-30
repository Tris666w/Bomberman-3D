#include "stdafx.h"
#include "BoneObject.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

BoneObject::BoneObject(int boneId, int materialId, float length)
	:m_BoneId(boneId)
	,m_MaterialId(materialId)
	,m_Length(length)
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
	
	pBone->GetTransform()->Translate(m_Length,0.f,0.f);
	AddChild(pBone);
}

void BoneObject::Initialize(const GameContext& )
{
	auto *const pModelComponent = new ModelComponent(L"Resources/Meshes/Bone.ovm");
	pModelComponent->SetMaterial(m_MaterialId);
	auto* const bone = new GameObject();
	bone->AddComponent(pModelComponent);

	bone->GetTransform()->Scale(m_Length,m_Length,m_Length);
	bone->GetTransform()->Rotate(0,-90,0);
	AddChild(bone);
	
}
