#pragma once
#include <vector>
#include "EffectHelper.h"

class MeshFilterLoader;
class Material;
struct GameContext;
class ModelComponent;

struct VertexBufferData
{
	VertexBufferData():
		pDataStart(nullptr),
		pVertexBuffer(nullptr),
		BufferSize(0),
		VertexStride(0),
		VertexCount(0),
		IndexCount(0),
		InputLayoutID(0){}

	void* pDataStart;
	ID3D11Buffer* pVertexBuffer;
	UINT BufferSize;
	UINT VertexStride;
	UINT VertexCount;
	UINT IndexCount;
	UINT InputLayoutID;

	void VertexBufferData::Destroy()
	{
		free(pDataStart);
		SafeRelease(pVertexBuffer);
	}
};

class MeshFilter final
{
public:
	MeshFilter();
	~MeshFilter();

private:

	friend class MeshFilterLoader;
	friend class ModelComponent;

	int GetVertexBufferId(UINT inputLayoutId);
	void BuildVertexBuffer(const GameContext& gameContext, Material* pMaterial);
	void BuildIndexBuffer(const GameContext& gameContext);
	bool HasElement(ILSemantic element) { return (m_HasElement&static_cast<UINT>(element)) > 0 ? true : false; }
	const VertexBufferData& GetVertexBufferData(const GameContext& gameContext, Material* pMaterial);

	//VERTEX DATA
	UINT m_VertexCount, m_IndexCount, m_TexCoordCount;
	UINT m_HasElement;
	std::vector<DirectX::XMFLOAT3> m_Positions;
	std::vector<DirectX::XMFLOAT3> m_Normals;
	std::vector<DirectX::XMFLOAT3> m_Tangents;
	std::vector<DirectX::XMFLOAT3> m_Binormals;
	std::vector<DirectX::XMFLOAT2> m_TexCoords;
	std::vector<DirectX::XMFLOAT4> m_Colors;

	//INDEX DATA
	std::vector<DWORD> m_Indices;

	std::vector<VertexBufferData> m_VertexBuffers;
	ID3D11Buffer* m_pIndexBuffer;

	std::wstring m_MeshName;
	static DirectX::XMFLOAT4 m_DefaultColor;
	static DirectX::XMFLOAT4 m_DefaultFloat4;
	static DirectX::XMFLOAT3 m_DefaultFloat3;
	static DirectX::XMFLOAT2 m_DefaultFloat2;

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MeshFilter(const MeshFilter &obj);
	MeshFilter& operator=(const MeshFilter& obj);
};
