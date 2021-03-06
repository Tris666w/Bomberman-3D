#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "BoneObject.h"
#include "TransformComponent.h"
#include "../../Materials/ColorMaterial.h"

SoftwareSkinningScene_1::SoftwareSkinningScene_1():GameScene(L"SoftwareSkinningScene_1")
	,m_pBone0(nullptr)
	,m_pBone1(nullptr)
	,m_BoneRotation(0)
	,m_RotationSign(1)
{
}

void SoftwareSkinningScene_1::Initialize()
{
	auto *const pColorMat = new ColorMaterial();
	GetGameContext().pMaterialManager->AddMaterial(pColorMat,0);

	auto *const pRoot = new GameObject();
	m_pBone0 = new BoneObject(0,0,15);
	m_pBone1 = new BoneObject(1,0,15);
	
	m_pBone0->AddBone(m_pBone1);
	pRoot->AddChild(m_pBone0);

	AddChild(pRoot);
}

void SoftwareSkinningScene_1::Update()
{
	m_BoneRotation += m_RotationSign * 45 * GetGameContext().pGameTime->GetElapsed();

	if (m_RotationSign < 0 && m_BoneRotation <= -45)
		m_RotationSign = 1;
	if (m_RotationSign > 0 && m_BoneRotation >= 45)
		m_RotationSign = -1;

	m_pBone0->GetTransform()->Rotate(0,0,m_BoneRotation);
	m_pBone1->GetTransform()->Rotate(0,0,-m_BoneRotation * 2.f);
	
}

void SoftwareSkinningScene_1::Draw()
{
}
