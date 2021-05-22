#pragma once
#include "BaseComponent.h"
enum class EmitterShape
{
	Cone = 0,
	Sphere = 1
};

struct Burst
{
	Burst(int count = 10, int cycles = 10, float intervalTime = 0.1f, float triggerTime = 5.f) :
		Count(count),
		Cycles(cycles),
		IntervalTime(intervalTime),
		TriggerTime(triggerTime),
		PassedCycles(0),
		PassedIntervalTime(0.f),
		TotalTimePast(0.f)
	{

	}

	//Reset PassedCycles, TotalTime and PassedIntervalTime
	void Reset()
	{
		PassedCycles = 0;
		PassedIntervalTime = 0.f;
		TotalTimePast = 0.f;
	}
	//Amount of particles to spawn at a burst
	int Count;
	//How often to repeat, -1 for infinite
	int Cycles;
	//Time between bursts
	float IntervalTime;
	//The amount of that that has to have passed before the first burst
	float TriggerTime;
	//Amount of cycles that have passed
	int PassedCycles;
	//Time past in this interval
	float PassedIntervalTime;
	//Total lifetime of this burst
	float TotalTimePast;
};

struct ParticleEmitterSettings
{
	ParticleEmitterSettings() :
		EmitRate(10),
		MinSize(0.1f),
		MaxSize(2.0f),
		MinEnergy(1.0f),
		MaxEnergy(2.0f),
		Velocity(0.f),
		MaxEmitterRange(10.0f),
		MinEmitterRange(9.0f),
		MinSizeGrow(1.0f),
		MaxSizeGrow(1.0f),
		StartColor(static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White)),
		EndingColor(static_cast<DirectX::XMFLOAT4>(DirectX::Colors::Black)),
		m_Shape(EmitterShape::Cone),
		MoveDirection({ 0,1,0 })
	{
	}
	//The amount of particles spawned every second
	int EmitRate;
	//The minimum size each particle can be at the time when it is spawned
	float MinSize;
	//The maximum size each particle can be at the time when it is spawned
	float MaxSize;
	//The minimum lifetime of each particle, measured in seconds
	float MinEnergy;
	//The maximum lifetime of each particle, measured in seconds
	float MaxEnergy;
	//The starting speed of particles along X, Y and Z, measured in the object's orientation
	float Velocity;
	//The maximum radius that the particles are spawned in
	float MaxEmitterRange;
	//The minimum radius that the particles are spawned in
	float MinEmitterRange;
	//The percentual maximum change in size during the particle's lifetime
	float MinSizeGrow;
	//The percentual minimum change in size during the particle's lifetime
	float MaxSizeGrow;
	//The starting color of a particle
	DirectX::XMFLOAT4 StartColor;
	//The ending color of a particle
	DirectX::XMFLOAT4 EndingColor;
	//The shape of the emitter
	EmitterShape m_Shape;
	//The MoveDirection of a particle (if m_Shape == Cone)
	DirectX::XMFLOAT3 MoveDirection;
};

struct ParticleVertex
{
	ParticleVertex(const DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0),
		const DirectX::XMFLOAT4 col = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White),
		const float size = 5.0f, const float rotation = 0) :
		Position(pos),
		Color(col),
		Size(size),
		Rotation(rotation)
	{
	}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	float Size;
	float Rotation;
};

class Particle;
class TextureData;

class ParticleEmitterComponent : public BaseComponent
{
	//The actual vertex buffer, containing ParticleVertex information for each Particle
	ID3D11Buffer* m_pVertexBuffer;

	//The effect we'll use to render the particles
	ID3DX11Effect* m_pEffect;
	//The default technique
	ID3DX11EffectTechnique* m_pDefaultTechnique{};
	//Shader matrix variables
	ID3DX11EffectMatrixVariable* m_pWvpVariable{}, * m_pViewInverseVariable{};
	//Shader texture variable
	ID3DX11EffectShaderResourceVariable* m_pTextureVariable{};
	//ShaderResourceView, containing the particle texture
	TextureData* m_pParticleTexture;

	//Vertex input layout variables
	ID3D11InputLayout* m_pInputLayout;
	UINT m_pInputLayoutSize;

	//Vector of particle pointers
	std::vector<Particle*> m_Particles;
	//Vector of Burst pointers
	std::vector<Burst*> m_Bursts;
	//The settings for this particle system
	ParticleEmitterSettings m_Settings;
	//The total amount of particles (m_Particles.size() == m_ParticleCount)
	int m_ParticleCount;
	//The active particles for a certain frame
	int m_ActiveParticles;
	//Total seconds since the last particle initialisation
	float m_LastParticleInit;
	//string containing the path to the particle texture
	std::wstring m_AssetFile;
	//Bool to check if the emission is active
	bool m_IsEmissionActive;
	//Bool to check if the burst is active
	bool m_IsBurstActive;
	//Method to load effect-related stuff
	void LoadEffect(const GameContext& gameContext);
	//Method to create the vertex buffer
	void CreateVertexBuffer(const GameContext& gameContext);
	//Method to update the bursts
	void UpdateBursts(const GameContext& gameContext);


public:
	ParticleEmitterComponent(const ParticleEmitterComponent& other) = delete;
	ParticleEmitterComponent(ParticleEmitterComponent&& other) noexcept = delete;
	ParticleEmitterComponent& operator=(const ParticleEmitterComponent& other) = delete;
	ParticleEmitterComponent& operator=(ParticleEmitterComponent&& other) noexcept = delete;
	//Constructor, particle texture path and max particles
	ParticleEmitterComponent(std::wstring assetFile, int particleCount = 50);
	~ParticleEmitterComponent() override;

	//EmitterSettings SETTERS
	void SetEmitRate(int emitRate) { m_Settings.EmitRate = emitRate; }
	void SetMinSize(float minSize) { m_Settings.MinSize = minSize; }
	void SetMaxSize(float maxSize) { m_Settings.MaxSize = maxSize; }
	void SetMinEnergy(float minEnergy) { m_Settings.MinEnergy = minEnergy; }
	void SetMaxEnergy(float maxEnergy) { m_Settings.MaxEnergy = maxEnergy; }
	void SetVelocity(float velocity) { m_Settings.Velocity = velocity; }
	void SetMinSizeGrow(float minSizeGrow) { m_Settings.MinSizeGrow = minSizeGrow; }
	void SetMaxSizeGrow(float maxSizeGrow) { m_Settings.MaxSizeGrow = maxSizeGrow; }
	void SetMinEmitterRange(float minEmitterRange) { m_Settings.MinEmitterRange = minEmitterRange; }
	void SetMaxEmitterRange(float maxEmitterRange) { m_Settings.MaxEmitterRange = maxEmitterRange; }
	void SetStartingColor(DirectX::XMFLOAT4 color) { m_Settings.StartColor = color; }
	void SetEndingColor(DirectX::XMFLOAT4 color) { m_Settings.EndingColor = color; }
	void SetShape(EmitterShape shape) { m_Settings.m_Shape = shape; }
	void SetConeMoveDirection(DirectX::XMFLOAT3 moveDir) { m_Settings.MoveDirection = moveDir; }
	void SetEmissionActive(bool isActive) { m_IsEmissionActive = isActive; };
	void SetBurstActive(bool isActive) { m_IsBurstActive = isActive; };

	void AddBurst(Burst* pBurst);
	void RemoveBurst(Burst* pBurst);
	void ResetBursts();

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void PostDraw(const GameContext& gameContext) override;
};
