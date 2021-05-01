#include "stdafx.h"
#include "LinkGameObjectPosComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"

LinkGameObjectPosComponent::LinkGameObjectPosComponent(GameObject* pToFollow)
	:m_pToFollowGameObject(pToFollow)
{
}

void LinkGameObjectPosComponent::Initialize(const GameContext& )
{
}

void LinkGameObjectPosComponent::Update(const GameContext& )
{
	GetGameObject()->GetTransform()->Translate(m_pToFollowGameObject->GetTransform()->GetPosition());
}

void LinkGameObjectPosComponent::Draw(const GameContext& )
{
}
