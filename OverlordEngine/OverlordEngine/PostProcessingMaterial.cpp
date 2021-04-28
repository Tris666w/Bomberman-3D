#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
											   std::wstring technique)
	: m_IsInitialized(false), 
	  m_pInputLayout(nullptr),
	  m_pInputLayoutSize(0),
	  m_effectFile(std::move(effectFile)),
	  m_InputLayoutID(0),
	  m_RenderIndex(renderIndex),
	  m_pRenderTarget(nullptr),
	  m_pVertexBuffer(nullptr),
	  m_pIndexBuffer(nullptr),
	  m_NumVertices(0),
	  m_NumIndices(0),
	  m_VertexBufferStride(0),
	  m_pEffect(nullptr),
	  m_pTechnique(nullptr),
	  m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pTechnique);
	SafeRelease(m_pEffect);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
	SafeDelete(m_pRenderTarget);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		//TODO: complete
		//1. LoadEffect (LoadEffect(...))
		LoadEffect(gameContext,m_effectFile);
		
		//2. CreateInputLaytout (CreateInputLayout(...))
		EffectHelper::BuildInputLayout(gameContext.pDevice,m_pTechnique,&m_pInputLayout,
			m_pInputLayoutDescriptions,m_pInputLayoutSize,m_InputLayoutID);
		
		//CreateIndexBuffer(gameContext);
		CreateIndexBuffer(gameContext);
		
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		CreateVertexBuffer(gameContext);
		
		//3. Create RenderTarget (m_pRenderTarget)
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()

		m_pRenderTarget = new RenderTarget(gameContext.pDevice);
		RENDERTARGET_DESC desc;
		desc.EnableDepthSRV = false;
		desc.EnableDepthBuffer = true;
		desc.Width = OverlordGame::GetGameSettings().Window.Width;
		desc.Height = OverlordGame::GetGameSettings().Window.Height;
		desc.EnableColorSRV = true;
		desc.EnableColorBuffer = true;
		desc.GenerateMipMaps_Color = true;
		m_pRenderTarget->Create(desc);

		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& , const std::wstring& effectFile)
{
	//TODO: complete
	//Load Effect through ContentManager
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	
	//Check if m_TechniqueName (default constructor parameter) is set
	if (!m_TechniqueName.empty())
	{
		// If SET > Use this Technique (+ check if valid)
		m_pTechnique = m_pEffect->GetTechniqueByName(WstringToString(m_TechniqueName).c_str());
		if (!m_pTechnique->IsValid())
		{
			Logger::LogWarning((L"PostProcessingMaterial::LoadEffect, m_pTechnique with techhnique name: " +
				m_TechniqueName + L" is not valid, using technique with index 0 ")	,true);
			m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
		}
	}else
		// If !SET > Use Technique with index 0
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	

	//Call LoadEffectVariables
	LoadEffectVariables();
	
	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	//TODO: complete
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	float clearColor[4] = {0.2f,0.6f,0.8f, 1.0f};
	m_pRenderTarget->Clear(gameContext,clearColor);
	
	//2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRendertarget);
	
	//3. Set InputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	
	//4. Set VertexBuffer
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &offset);

	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer,DXGI_FORMAT_R32_UINT,0);
	
	//5. Set PrimitiveTopology (TRIANGLELIST)
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//6. Draw 
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (unsigned int i = 0; i < techDesc.Passes; ++i)
	{
		m_pTechnique->GetPassByIndex(i)->Apply(0,gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices,0,0);
	}
	
	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	m_NumVertices = 4;
	
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//Create vertex array containing three elements in system memory
	using namespace DirectX;
	std::vector<VertexPosTex> vertices = {};
	
	//Topleft
	vertices.push_back(VertexPosTex(XMFLOAT3(-1,1,0),XMFLOAT2(0,0)));
	//TopRight
	vertices.push_back(VertexPosTex(XMFLOAT3(1,1,0),XMFLOAT2(1,0)));
	//BottomRight
	vertices.push_back(VertexPosTex(XMFLOAT3(1,-1,0),XMFLOAT2(1,1)));
	//Bottomleft
	vertices.push_back(VertexPosTex(XMFLOAT3(-1,-1,0),XMFLOAT2(0,1)));

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = vertices.data();
	
	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = m_NumVertices * sizeof(VertexPosTex);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	m_VertexBufferStride = sizeof(VertexPosTex);
	
	//create a ID3D10Buffer in graphics memory containing the vertex info
	auto hr = gameContext.pDevice->CreateBuffer(&desc,&initialData,&m_pVertexBuffer);
	Logger::LogHResult(hr, L"PostProcessingMaterial::CreateVertexBuffer()");
	
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	m_NumIndices = 6;
	//TODO: complete
	// Create index buffer
	D3D11_BUFFER_DESC desc = {};
	desc.CPUAccessFlags = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(unsigned int) * m_NumIndices;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	std::vector<unsigned int> indices= {0,1,3,3,1,2};
	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = indices.data();
	
	auto hr = gameContext.pDevice->CreateBuffer(&desc,&initialData,&m_pIndexBuffer);
	Logger::LogHResult(hr, L"PostProcessingMaterial::CreateIndexBuffer");
}
