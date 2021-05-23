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
	//Fire emitter
	auto* pBurst = new Burst(25, -1, 3.f, 2.f);

	auto const scale = GetTransform()->GetScale();
	auto const avgScale = (scale.x + scale.y + scale.z) / 3.f;
	auto* pParticleComp = new ParticleEmitterComponent(L"Resources/Textures/Particles/FireBall.png", 40);
	pParticleComp->SetEmitRate(20);
	pParticleComp->SetVelocity(6.f);
	pParticleComp->SetMinSize(1.0f * avgScale);
	pParticleComp->SetMaxSize(2.0f * avgScale);
	pParticleComp->SetMinEnergy(0.75f * avgScale);
	pParticleComp->SetMaxEnergy(1.25f * avgScale);
	pParticleComp->SetMinSizeGrow(0.4f * avgScale);
	pParticleComp->SetMaxSizeGrow(1.5f * avgScale);
	pParticleComp->SetMinEmitterRange(0.5f * avgScale);
	pParticleComp->SetMaxEmitterRange(1.f * avgScale);
	pParticleComp->SetStartingColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f));
	pParticleComp->SetEndingColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f));
	pParticleComp->SetShape(EmitterShape::Cone);
	pParticleComp->AddBurst(pBurst);
	AddComponent(pParticleComp);


	//Smoke emitter
	pBurst = new Burst(5,-1,3.1f,2.f);
	pParticleComp = new ParticleEmitterComponent(L"Resources/Textures/Particles/smoke_04.png",25);
	pParticleComp->SetShape(EmitterShape::Cone);
	pParticleComp->SetStartingColor(DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.8f));
	pParticleComp->SetEndingColor(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, .5f));
	
	pParticleComp->SetVelocity(6.f);
	pParticleComp->SetMinSize(0.0f * avgScale);
	pParticleComp->SetMaxSize(.8f * avgScale);
	pParticleComp->SetMinEnergy(1.5f * avgScale);
	pParticleComp->SetMaxEnergy(2.0f * avgScale);
	pParticleComp->SetMinSizeGrow(1.f * avgScale);
	pParticleComp->SetMaxSizeGrow(2.5f * avgScale);
	pParticleComp->SetMinEmitterRange(0.2f * avgScale);
	pParticleComp->SetMaxEmitterRange(0.5f * avgScale);
	pParticleComp->SetEmitRate(20);
	pParticleComp->AddBurst(pBurst);
	AddComponent(pParticleComp);

	gameContext.pShadowMapper->SetLight({ -100,150,-50 }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Model
	auto* const pModelComp = new ModelComponent(L"Resources/Meshes/Bonfire.ovm");
	auto* const pMat = new DiffuseMaterial_Shadow();
	auto const matId = gameContext.pMaterialManager->AddMaterial(pMat);
	pMat->SetDiffuseTexture(L"Resources/Textures/Vegetation/Log.png");
	pMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	pModelComp->SetMaterial(matId);
	AddComponent(pModelComp);


}

void BonfirePrefab::Draw(const GameContext&)
{
}

void BonfirePrefab::Update(const GameContext&)
{
}
