#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter) :
	m_pMeshFilter(pMeshFilter),
	m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
	m_IsPlaying(false),
	m_Reversed(false),
	m_ClipSet(false),
	m_TickCount(0),
	m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;
	if (clipNumber > m_pMeshFilter->m_AnimationClips.size())
	{
		Reset();
		Logger::LogWarning(L" ModelAnimator::SetAnimation(UINT clipNumber) -> clipNumber > m_AnimationClips.size");
		return;
	}
	else
	{
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	m_ClipSet = false;
	bool foundAnim = false;
	for (const AnimationClip& animClip : m_pMeshFilter->m_AnimationClips)
	{
		if (clipName == animClip.Name)
		{
			SetAnimation(animClip);
			foundAnim = true;
		}
	}
	if (!foundAnim)
	{
		Reset();
		Logger::LogWarning(L" ModelAnimator::SetAnimation(std::wstring clipName) -> No clip with given name found");
	}
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	if (pause)
	{
		m_IsPlaying = false;
	}
	m_TickCount = 0.f;
	m_AnimationSpeed = 1.0f;

	if (m_ClipSet)
	{
		auto boneTransformVect = m_CurrentClip.Keys[0].BoneTransforms;
		m_Transforms.assign(boneTransformVect.begin(), boneTransformVect.end());
	}
	else
	{
		DirectX::XMFLOAT4X4 identity{};
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	if (m_IsPlaying && m_ClipSet)
	{
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		if (passedTicks > m_CurrentClip.Duration)
		{
			passedTicks = m_CurrentClip.Duration;
		}

		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
				m_TickCount += m_CurrentClip.Duration;
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration)
				m_TickCount -= m_CurrentClip.Duration;
		}

		AnimationKey keyA, keyB;
		float tickDiff = 0;
		float previousTickDiff = FLT_MAX;
		for (const AnimationKey& currentKey : m_CurrentClip.Keys)
		{
			tickDiff = m_TickCount - currentKey.Tick;
			if (tickDiff > 0.f)
			{
				keyA = currentKey;
				continue;
			}
			tickDiff = currentKey.Tick - m_TickCount;
			if (tickDiff > 0.f && previousTickDiff > tickDiff)
			{
				keyB = currentKey;
				previousTickDiff = tickDiff;
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		tickDiff = keyB.Tick - keyA.Tick;
		float const currentRelativeTick = m_TickCount - keyA.Tick;
		float blendWeight = (tickDiff / currentRelativeTick) / 100.f;

		//Clear the m_Transforms vector
		m_Transforms.clear();

		for (int index = 0; index < m_pMeshFilter->m_BoneCount; ++index)
		{
			auto const transformA = DirectX::XMLoadFloat4x4(&keyA.BoneTransforms[index]);
			auto const transformB = DirectX::XMLoadFloat4x4(&keyB.BoneTransforms[index]);

			DirectX::XMVECTOR positionA{};
			DirectX::XMVECTOR positionB{};
			DirectX::XMVECTOR scaleA{};
			DirectX::XMVECTOR scaleB{};
			DirectX::XMVECTOR rotationA{};
			DirectX::XMVECTOR rotationB{};

			DirectX::XMMatrixDecompose(&scaleA, &rotationA, &positionA, transformA);
			DirectX::XMMatrixDecompose(&scaleB, &rotationB, &positionB, transformB);

			auto const interpolatedPos = DirectX::XMVectorLerp(positionA, positionB, blendWeight);
			auto const interpolatedRot = DirectX::XMQuaternionSlerp(rotationA, rotationB, blendWeight);
			auto const interpolatedScale = DirectX::XMVectorLerp(scaleA, scaleB, blendWeight);

			auto const translationMatrix = DirectX::XMMatrixTranslationFromVector(interpolatedPos);
			auto const rotationMatrix = DirectX::XMMatrixRotationQuaternion(interpolatedRot);
			auto const scaleMatrix = DirectX::XMMatrixScalingFromVector(interpolatedScale);

			auto const transformXMMatrix = rotationMatrix * translationMatrix * scaleMatrix;

			DirectX::XMFLOAT4X4 transformMatrix{};
			DirectX::XMStoreFloat4x4(&transformMatrix, transformXMMatrix);
			m_Transforms.push_back(transformMatrix);

		}
	}
}
