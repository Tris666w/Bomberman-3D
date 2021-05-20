#include "stdafx.h"

#include "SpriteComponent.h"
 #include <utility>

#include "GameObject.h"
#include "TextureData.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TransformComponent.h"

SpriteComponent::SpriteComponent(std::wstring spriteAsset, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color):
	m_pTexture(nullptr),
	m_SpriteAsset(std::move(spriteAsset)),
	m_Pivot(pivot),
	m_Color(color)
{
}

DirectX::XMFLOAT2 SpriteComponent::GetTextureDimensions() const
{
	return m_pTexture->GetDimension();
}

void SpriteComponent::Initialize(const GameContext& )
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Update(const GameContext& )
{
}

void SpriteComponent::Draw(const GameContext&)
{
	if (!m_pTexture)
		return;

	// The sprite renderer is a singleton
	// you will need to position, the rotation and the scale
	// You can use the QuaternionToEuler function to help you with the z rotation
	TransformComponent* pTransform = GetGameObject()->GetTransform();

	DirectX::XMFLOAT2 const pos = { pTransform->GetWorldPosition().x,pTransform->GetWorldPosition().y };
	DirectX::XMFLOAT2 const scale = { pTransform->GetScale().x,pTransform->GetScale().y };
	float const rotation = QuaternionToEuler(pTransform->GetRotation()).z;
	float const depth = pTransform->GetPosition().z;
	
	SpriteRenderer::GetInstance()->Draw(m_pTexture, pos, m_Color, m_Pivot, scale, rotation, depth);
}
