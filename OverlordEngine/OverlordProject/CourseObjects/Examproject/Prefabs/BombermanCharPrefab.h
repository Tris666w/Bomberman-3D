#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;
class ModelComponent;
class BombPrefab;

class BombermanCharPrefab : public GameObject
{
public:
	/// @brief Constructor of the bomberman character
	/// @param meshFilePath path to the .ovm mesh file
	/// @param materialFilePath path to the diffuse map
	/// @param radius radius of the characterController
	/// @param height  height of the characterController
	/// @param stepOffset the maximum height the character can climb
	/// @param moveSpeed the movement speed
	/// @param controlKeyVect Vector of the input keys
	/// @param playerIndex the index of the player
	BombermanCharPrefab(const std::wstring& meshFilePath, const std::wstring& materialFilePath, float radius = 2, float height = 5, float stepOffset = 0.5f, float moveSpeed = 100, const std::vector<int>& controlKeyVect = std::vector<int>({ 'A', 'D', 'W', 'S', 32 }), GamepadIndex playerIndex = GamepadIndex::PlayerOne);
	virtual ~BombermanCharPrefab() = default;

	BombermanCharPrefab(const BombermanCharPrefab& other) = delete;
	BombermanCharPrefab(BombermanCharPrefab&& other) noexcept = delete;
	BombermanCharPrefab& operator=(const BombermanCharPrefab& other) = delete;
	BombermanCharPrefab& operator=(BombermanCharPrefab&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void KillPlayer();

protected:
	bool m_IsDead;
	ControllerComponent* m_pController;
	ModelComponent* m_pModel = nullptr;
	float m_Radius, m_Height, m_StepOffset;
	std::vector<int> m_InputKeys{};
	std::vector<int> m_ActionIDs{};
	GamepadIndex const m_PlayerIndex;
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

	//Bomb parameters
	float const m_BombCooldown = 3.f;
	float m_BombTimer = 0.f;
	bool m_CanSpawnBomb = true;

	//File paths
	std::wstring m_MeshFilePath;
	std::wstring m_MaterialFilePath;

	//Methods
	DirectX::XMFLOAT3 CalculateBombSpawnPos()const;
};

