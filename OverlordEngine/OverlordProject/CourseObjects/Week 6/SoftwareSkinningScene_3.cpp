#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "BoneObject.h"
#include "TransformComponent.h"
#include "../../Materials/ColorMaterial.h"
#include "SkinnedVertex.h"
#include "MeshDrawComponent.h"

SoftwareSkinningScene_3::SoftwareSkinningScene_3():GameScene(L"SoftwareSkinningScene_3")
	,m_pBone0(nullptr)
	,m_pBone1(nullptr)
	,m_BoneRotation(0)
	,m_RotationSign(1)
	,m_pMeshDrawer(nullptr)
	,m_SkinnedVertices()
{
}

void SoftwareSkinningScene_3::Initialize()
{
	auto *const pColorMat = new ColorMaterial();
	GetGameContext().pMaterialManager->AddMaterial(pColorMat,0);

	auto *const pRoot = new GameObject();
	float const boneLength = 15.f;
	m_pBone0 = new BoneObject(0,0,boneLength);
	m_pBone1 = new BoneObject(1,0,boneLength);
	
	m_pBone0->AddBone(m_pBone1);
	pRoot->AddChild(m_pBone0);
	AddChild(pRoot);
	m_pBone0->CalculateBindPose();

	auto* const pMeshDraw = new GameObject();
	m_pMeshDrawer = new MeshDrawComponent(24);
	pMeshDraw->AddComponent(m_pMeshDrawer);
	CreateMesh(boneLength);

	AddChild(pMeshDraw);
}

void SoftwareSkinningScene_3::Update()
{
	using namespace DirectX;
	
	m_BoneRotation += m_RotationSign * 45 * GetGameContext().pGameTime->GetElapsed();

	if (m_RotationSign < 0 && m_BoneRotation <= -45)
		m_RotationSign = 1;
	if (m_RotationSign > 0 && m_BoneRotation >= 45)
		m_RotationSign = -1;

	m_pBone0->GetTransform()->Rotate(0,0,m_BoneRotation);
	m_pBone1->GetTransform()->Rotate(0,0,-m_BoneRotation * 2.f);

	XMFLOAT4X4 bone0Transform = {};
	XMFLOAT4X4 bone1Transform = {};

	auto bone0Matrix = XMLoadFloat4x4(&bone0Transform);
	auto bone1Matrix = XMLoadFloat4x4(&bone1Transform);

	auto bindPose0 = m_pBone0->GetBindPose();
	auto bindPose1 = m_pBone1->GetBindPose();
	
	auto const bone0Bind = XMLoadFloat4x4(&bindPose0);
	auto const bone1Bind = XMLoadFloat4x4(&bindPose1);


	auto const world0 = m_pBone0->GetTransform()->GetWorld();
	auto const world1 = m_pBone1->GetTransform()->GetWorld();
	
	auto const bone0World = XMLoadFloat4x4(&world0);
	auto const bone1World = XMLoadFloat4x4(&world1);
	
	bone0Matrix = bone0Bind * bone0World;
	bone1Matrix = bone1Bind * bone1World;
	
	for (size_t index = 0; index < m_SkinnedVertices.size(); index++)
	{
		auto const originalVertexPos = XMLoadFloat3(&m_SkinnedVertices[index].OriginalVertex.Position);
		auto const transformedVertex = XMVector3TransformCoord(originalVertexPos, (index<=24)? bone0Matrix : bone1Matrix);
		auto const otherTransformedVertex = XMVector3TransformCoord(originalVertexPos, (index>24)? bone0Matrix : bone1Matrix);
		
		auto const blendedPos = m_SkinnedVertices[index].BlendWeight1 * transformedVertex + m_SkinnedVertices[index].BlendWeight2 * otherTransformedVertex;
		XMStoreFloat3(&m_SkinnedVertices[index].TransformedVertex.Position,blendedPos);
	}

	m_pMeshDrawer->RemoveTriangles();
	for (size_t index = 0; index < m_SkinnedVertices.size(); index += 4)
	{
		auto newQuad = QuadPosNormCol(m_SkinnedVertices[index].TransformedVertex,
									m_SkinnedVertices[index+1].TransformedVertex,
									m_SkinnedVertices[index+2].TransformedVertex,
									m_SkinnedVertices[index+3].TransformedVertex);
		m_pMeshDrawer->AddQuad(newQuad,true);
	}
	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::Draw()
{
}

void SoftwareSkinningScene_3::CreateMesh(float length)
{
	auto pos = DirectX::XMFLOAT3(length/2, 0, 0);
	auto offset = DirectX::XMFLOAT3(length/2, 2.5f, 2.5f);
	auto col = DirectX::XMFLOAT4(1, 0, 0, 0.5f);

	//***** //BOX1* //*****
	//FRONT
	auto norm = DirectX::XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
	
	//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	
	//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col));
	
	//BOTTOM
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));

	//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));

	//RIGHT
	norm = DirectX::XMFLOAT3(1, 0, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	
	//***** //BOX2* //*****
	
	col = DirectX::XMFLOAT4(0, 1, 0, 0.5f);
	pos = DirectX::XMFLOAT3(22.5f, 0, 0);

	//FRONT
	norm = DirectX::XMFLOAT3(0, 0, -1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));

	//BACK
	norm = DirectX::XMFLOAT3(0, 0, 1);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));
	
	//TOP
	norm = DirectX::XMFLOAT3(0, 1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f)); 
	
	//BOTTOM 
	norm = DirectX::XMFLOAT3(0, -1, 0);
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f));

	//LEFT
	norm = DirectX::XMFLOAT3(-1, 0, 0); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col,0.5f,0.5f)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(-offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col,0.5f,0.5f)); 
	
	//RIGHT 
	norm = DirectX::XMFLOAT3(1, 0, 0); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z), norm, col)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, offset.y + pos.y, offset.z + pos.z), norm, col)); 
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z), norm, col));
	m_SkinnedVertices.push_back(SkinnedVertex(DirectX::XMFLOAT3(offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z), norm, col));
}
