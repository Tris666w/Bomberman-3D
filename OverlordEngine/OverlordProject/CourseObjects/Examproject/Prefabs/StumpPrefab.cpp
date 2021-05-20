#include "stdafx.h"
#include "StumpPrefab.h"

#include "ColliderComponent.h"
#include "GameScene.h"
#include "ModelComponent.h"
#include "PhysxManager.h"
#include "RigidBodyComponent.h"
#include "SoundManager.h"
#include "../BombermanGameSettings.h"

StumpPrefab::StumpPrefab(int matId)
	:GameObject(),
	m_MatId(matId)
{
}

StumpPrefab::~StumpPrefab()
{
}

void StumpPrefab::Break()
{
	//Sound
	FMOD::Sound* pSound;
	FMOD::Channel* pChannel;
	auto const pFmodSystem = SoundManager::GetInstance()->GetSystem();
	auto const fmodResult = pFmodSystem->createStream("Resources/Sounds/WoodBreak.wav", FMOD_DEFAULT, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	pSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->ErrorCheck(fmodResult);
	SoundManager::GetInstance()->GetSystem()->playSound(pSound, nullptr, false, &pChannel);
	pChannel->setVolume(BombermanGameSettings::GetInstance()->GetSoundVolume());
	GetScene()->RemoveChild(this, true);
}

void StumpPrefab::Initialize(const GameContext&)
{
	auto const bouncyMaterial = PhysxManager::GetInstance()->GetPhysics()->createMaterial(0, 0, 1.f);
	auto const halfSize = BombermanGameSettings::GetInstance()->GetBlockSize() / 2.f;

	auto* pModelComponent = new ModelComponent(L"./Resources/Meshes/TreeStump.ovm");
	pModelComponent->SetMaterial(m_MatId);
	AddComponent(pModelComponent);

	SetTag(BombermanGameSettings::GetInstance()->GetDestructibleTag());

	auto const rb = new RigidBodyComponent();
	rb->SetKinematic(true);
	rb->SetCollisionGroup(CollisionGroupFlag::Group0);
	AddComponent(rb);

	std::shared_ptr<physx::PxGeometry>geometry
		(new physx::PxBoxGeometry(static_cast<float>(BombermanGameSettings::GetInstance()->GetBlockSize()), halfSize, halfSize));
	
	auto* cc = new ColliderComponent(geometry, *bouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1))));

	AddComponent(cc);
}

void StumpPrefab::Draw(const GameContext&)
{
}

void StumpPrefab::Update(const GameContext&)
{
}

void StumpPrefab::PostInitialize(const GameContext&)
{
	GetComponent<RigidBodyComponent>()->GetPxRigidBody()->userData = this;

}
