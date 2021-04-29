#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"
#include "GameScene.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowMat);
	SafeDelete(m_pShadowRT);

}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);

	m_pShadowRT = new RenderTarget(gameContext.pDevice);
	RENDERTARGET_DESC descriptor{};
	auto window = OverlordGame::GetGameSettings().Window;
	descriptor.Width = window.Width;
	descriptor.Height = window.Height;

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

	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	
	gameContext.pDeviceContext->ClearDepthStencilView(m_pShadowRT->GetDepthStencilView(),D3D11_CLEAR_DEPTH,1.0f,0);
	float clearColor[4] = {0.6f,0.6f,0.6f,1.f};

	auto* renderTargetView = m_pShadowRT->GetRenderTargetView();
	if (renderTargetView)
	{
		gameContext.pDeviceContext->ClearRenderTargetView(renderTargetView,clearColor);	
	}
	
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& ) const
{
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	m_pShadowMat->SetBones(&bones.data()->_11,bones.size());
	m_pShadowMat->SetWorld(world);
	
	int type = -1;
	
	if (pMeshFilter->m_HasAnimations)
		type = ShadowMapMaterial::ShadowGenType::Skinned;
	else
		type = ShadowMapMaterial::ShadowGenType::Static;
	
	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[type]);
	
	auto vertexBufferData = pMeshFilter->GetVertexBufferData(gameContext,m_pShadowMat->m_InputLayoutIds[type]);
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0,1,&vertexBufferData.pVertexBuffer,&vertexBufferData.VertexStride,&offset);
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer,DXGI_FORMAT_R32_UINT,0);

	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
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
	int type = -1;
	if (pMeshFilter->m_HasAnimations)
		type =  ShadowMapMaterial::ShadowGenType::Skinned;
	else
		type =  ShadowMapMaterial::ShadowGenType::Static;

	pMeshFilter->BuildVertexBuffer(gameContext,m_pShadowMat->m_InputLayoutIds[type],
		m_pShadowMat->m_InputLayoutSizes[type],m_pShadowMat->m_InputLayoutDescriptions[type]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRT->GetDepthShaderResourceView();
}
