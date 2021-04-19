#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "GameScene.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: create shadow generator material + initialize it
	//TODO: create a rendertarget with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);

	m_pShadowRT = new RenderTarget(gameContext.pDevice);
	RENDERTARGET_DESC descriptor{};
	descriptor.Width = int(m_Size);
	descriptor.Height = int(m_Size);
	
	descriptor.EnableDepthBuffer = true;
	descriptor.EnableDepthSRV = true;
	descriptor.EnableColorBuffer = false;
	descriptor.EnableColorSRV = false;
	m_pShadowRT->Create(descriptor);

	
	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	using namespace DirectX;
	//TODO: store the input parameters in the appropriate datamembers
	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember
	m_LightPosition = position;
	m_LightDirection = direction;

	DirectX::XMFLOAT4X4 newLVP{};
	auto window = OverlordGame::GetGameSettings().Window;
	
	XMMATRIX projection;

	const float viewWidth = (m_Size > 0) ? m_Size * window.AspectRatio : window.Width;
	const float viewHeight = (m_Size > 0) ? m_Size : window.Height;
	projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, 1.f, 500.f);
	
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);

	const auto right = XMVector3Cross(dir,XMVectorSet(0.f,1.f,0.f,0.f));
	const auto up = XMVector3Cross(dir,right);
	
	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(pos, pos + dir, up);
	const DirectX::XMMATRIX viewProjection = view * projection;

	XMStoreFloat4x4(&m_LightVP,viewProjection);
}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	
	//TODO: clear this render target
	gameContext.pDeviceContext->ClearDepthStencilView(m_pShadowRT->GetDepthStencilView(),D3D11_CLEAR_DEPTH,1.0f,0);
	float clearColor[4] = {0.6f,0.6f,0.6f,1.f};
	gameContext.pDeviceContext->ClearRenderTargetView(m_pShadowRT->GetRenderTargetView(),clearColor);
	
	//TODO: set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
	
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);

	//TODO: update shader variables in material
	m_pShadowMat->SetBones(&bones.data()->_11,bones.size());
	m_pShadowMat->SetWorld(world);
	//TODO: set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)
	int type = -1;
	if (pMeshFilter->m_HasAnimations)
		type = ShadowMapMaterial::ShadowGenType::Skinned;
	else
		type = ShadowMapMaterial::ShadowGenType::Static;
	
	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[type]);
	
	auto vertexBufferData = pMeshFilter->GetVertexBufferData(gameContext,m_pShadowMat->m_InputLayoutIds[type]);
	gameContext.pDeviceContext->IASetVertexBuffers(0,1,&vertexBufferData.pVertexBuffer,&vertexBufferData.VertexStride,nullptr);
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer,DXGI_FORMAT_R32_UINT,0);

	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//TODO: invoke draw call
	D3DX11_TECHNIQUE_DESC techniqueDesc;
	m_pShadowMat->m_pShadowTechs[type]->GetDesc(&techniqueDesc);
	for (unsigned int index = 0; index< techniqueDesc.Passes; ++index)
	{
		m_pShadowMat->m_pShadowTechs[type]->GetPassByIndex(index)->Apply(0,gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount,0,0);
	}
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
	int type = -1;
	if (pMeshFilter->m_HasAnimations)
		type = 0;
	else
		type = 1;

	pMeshFilter->BuildVertexBuffer(gameContext,m_pShadowMat->m_InputLayoutIds[type],
		m_pShadowMat->m_InputLayoutSizes[type],m_pShadowMat->m_InputLayoutDescriptions[type]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetShaderResourceView();
}
