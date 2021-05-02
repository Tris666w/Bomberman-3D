#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;
class ModelComponent;
class BombPrefab;

class BombermanCharPrefab : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT = 1,
		FORWARD = 2,
		BACKWARD = 3,
		DROP_BOMB = 4
	};
	
	BombermanCharPrefab(const std::wstring& meshFilePath, const std::wstring& materialFilePath, float radius = 2, float height = 5,float stepOffset = 0.5f, float moveSpeed = 100);
	virtual ~BombermanCharPrefab() = default;

	BombermanCharPrefab(const BombermanCharPrefab& other) = delete;
	BombermanCharPrefab(BombermanCharPrefab&& other) noexcept = delete;
	BombermanCharPrefab& operator=(const BombermanCharPrefab& other) = delete;
	BombermanCharPrefab& operator=(BombermanCharPrefab&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;


protected:
	ControllerComponent* m_pController;
	ModelComponent* m_pModel = nullptr;
	float m_MoveSpeed;
	float m_Radius, m_Height, m_StepOffset;
	
	//Running
	float m_MaxRunVelocity, 
		m_TerminalVelocity,
		m_Gravity, 
		m_RunAccelerationTime, 
		m_JumpAccelerationTime, 
		m_RunAcceleration, 
		m_JumpAcceleration, 
		m_RunVelocity, 
		m_JumpVelocity;

	DirectX::XMFLOAT3 m_Velocity;

	//File paths
	std::wstring m_MeshFilePath;
	std::wstring m_MaterialFilePath;
	
	//Methods
	DirectX::XMFLOAT3 CalculateBombSpawnPos()const;
};

