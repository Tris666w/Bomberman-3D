#include "stdafx.h"
#include "DestroyableWallComponent.h"

#include "GameObject.h"
#include "TransformComponent.h"

void DestroyableWallComponent::DestroyObject()const
{
	auto* pParent = GetGameObject();
	pParent->GetTransform()->Translate(-10000, -10000, -10000);
}

void DestroyableWallComponent::Initialize(const GameContext&)
{
}

void DestroyableWallComponent::Update(const GameContext&)
{
}

void DestroyableWallComponent::Draw(const GameContext&)
{
}
