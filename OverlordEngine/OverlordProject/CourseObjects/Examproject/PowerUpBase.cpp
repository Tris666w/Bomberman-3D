#include "stdafx.h"
#include "PowerUpBase.h"

#include "BombermanGameSettings.h"
#include "ColliderComponent.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "Prefabs/BombermanCharPrefab.h"
#include "ParticleEmitterComponent.h"
#include "SoundManager.h"

void PowerUpBase::SetParticleEmitter(ParticleEmitterComponent* pEmitter)
{
	if (m_pEmitter)
	{
		RemoveComponent(m_pEmitter);
		SafeDelete(m_pEmitter);
	}
	m_pEmitter = pEmitter;
	AddComponent(m_pEmitter);

	m_pEmitter->SetEmissionActive(m_IsEnabled);
	m_pEmitter->SetBurstActive(m_IsEnabled);
}

void PowerUpBase::SetSound(FMOD::Sound* sound)
{
	m_pSound = sound;
}

void PowerUpBase::SetIsEnabled(bool enabled)
{
	m_IsEnabled = enabled;
	if (m_pEmitter)
	{
		m_pEmitter->SetEmissionActive(enabled);
		m_pEmitter->SetBurstActive(enabled);
	}
}

void PowerUpBase::PlayEffectSound() const
{
	if (m_pSound)
	{
		FMOD::Channel* channel;
		SoundManager::GetInstance()->GetSystem()->playSound(m_pSound, nullptr, false, &channel);
		channel->setVolume(BombermanGameSettings::GetInstance()->GetSoundVolume());
	}
	else
		Logger::LogInfo(L"PowerUpBase::PlayEffectSound(), No sound available");

}

void PowerUpBase::Init()
{
	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto const bouncyMaterial = physx->createMaterial(0, 0, 1.f);

	auto rb = new RigidBodyComponent(false, true);
	rb->SetCollisionGroup(CollisionGroupFlag::Group0);
	AddComponent(rb);

	physx::PxConvexMesh* const pxConvexMesh = ContentManager::Load<physx::PxConvexMesh>(L"Resources/Meshes/Cube.ovpc");
	std::shared_ptr<physx::PxGeometry> meshGeometry(new physx::PxConvexMeshGeometry(pxConvexMesh));

	auto cc = new ColliderComponent(meshGeometry, *bouncyMaterial);
	cc->EnableTrigger(true);

	SetOnTriggerCallBack([this](GameObject* trigger, GameObject* other, GameObject::TriggerAction action)
		{

			auto* const pChar = dynamic_cast<BombermanCharPrefab*>(other);
			if (action == TriggerAction::ENTER && pChar)
			{
				ExecuteEffect(pChar);
			}
			UNREFERENCED_PARAMETER(trigger);
		});
	AddComponent(cc);
}
