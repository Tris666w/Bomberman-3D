#include "stdafx.h"
#include "BonfirePrefab.h"
#include "ParticleEmitterComponent.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "../../../Materials/Shadow/DiffuseMaterial_Shadow.h"

BonfirePrefab::BonfirePrefab(DirectX::XMFLOAT3 spawnPos)
{
	GetTransform()->Translate(spawnPos);
}

void BonfirePrefab::Initialize(const GameContext& gameContext)
{
	auto* obj = new GameObject();
	
	//Fire emitter
	auto* pParticleComp = new ParticleEmitterComponent(L"Resources/Textures/FireBall.png",100);
	pParticleComp->SetEmitRate(20);
	pParticleComp->SetVelocity(6.f);
	pParticleComp->SetMinSize(1.0f);
	pParticleComp->SetMaxSize(2.0f);
	pParticleComp->SetMinEnergy(0.75f);
	pParticleComp->SetMaxEnergy(1.25f);
	pParticleComp->SetMinSizeGrow(0.4f);
	pParticleComp->SetMaxSizeGrow(1.5f);
	pParticleComp->SetMinEmitterRange(0.5f);
	pParticleComp->SetMaxEmitterRange(1.f);
	pParticleComp->SetStartingColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	pParticleComp->SetEndingColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	pParticleComp->SetShape(EmitterShape::Cone);
	obj->AddComponent(pParticleComp);
	pParticleComp->GetTransform()->Translate(0.f,0.f,0.f);


	//Smoke emitter
	pParticleComp = new ParticleEmitterComponent(L"Resources/Textures/smoke_04.png");
	pParticleComp->SetShape(EmitterShape::Cone);
	pParticleComp->SetStartingColor(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.8f));
	pParticleComp->SetEndingColor(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, .5f));
	
	pParticleComp->SetVelocity(6.f);
	pParticleComp->SetMinSize(0.0f);
	pParticleComp->SetMaxSize(.8f);
	pParticleComp->SetMinEnergy(1.5f);
	pParticleComp->SetMaxEnergy(2.0f);
	pParticleComp->SetMinSizeGrow(1.f);
	pParticleComp->SetMaxSizeGrow(2.5f);
	pParticleComp->SetMinEmitterRange(0.2f);
	pParticleComp->SetMaxEmitterRange(0.5f);
	pParticleComp->SetEmitRate(20);
	AddComponent(pParticleComp);
	pParticleComp->GetTransform()->Translate(0.f,0.f,0.f);

	gameContext.pShadowMapper->SetLight({ -100,150,-50 }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Model
	auto* const pModelComp = new ModelComponent(L"Resources/Meshes/Bonfire.ovm");
	auto* const pMat = new DiffuseMaterial_Shadow();
	auto const matId = gameContext.pMaterialManager->AddMaterial(pMat);
	pMat->SetDiffuseTexture(L"Resources/Textures/Log.png");
	pMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	pModelComp->SetMaterial(matId);
	AddComponent(pModelComp);
	AddChild(obj);
}

void BonfirePrefab::Draw(const GameContext&)
{
}

void BonfirePrefab::Update(const GameContext&)
{
}
