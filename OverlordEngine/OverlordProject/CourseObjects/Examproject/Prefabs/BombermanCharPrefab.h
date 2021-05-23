#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;
class ModelComponent;
class BombPrefab;
class SpriteFont;
class SpriteComponent;

class BombermanCharPrefab : public GameObject
{
public:
	/// @brief Constructor of the bomberman character
	/// @param meshFilePath path to the .ovm mesh file
	/// @param materialFilePath path to the diffuse map
	/// @param controlKeyVect Vector of the input keys
	/// @param playerIndex the index of the player
	/// @param useGamePad bool to check if we use a game pad
	BombermanCharPrefab(const std::wstring& meshFilePath, const std::wstring& materialFilePath, const std::vector<int>& controlKeyVect = std::vector<int>({ 'A', 'D', 'W', 'S', 32 }), GamepadIndex playerIndex = GamepadIndex::PlayerOne, bool useGamePad = false);
	~BombermanCharPrefab() override = default;

	BombermanCharPrefab(const BombermanCharPrefab& other) = delete;
	BombermanCharPrefab(BombermanCharPrefab&& other) noexcept = delete;
	BombermanCharPrefab& operator=(const BombermanCharPrefab& other) = delete;
	BombermanCharPrefab& operator=(BombermanCharPrefab&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext&) override;

	void DamagePlayer(int amount);
	void SetAmountOfBombs(int amount);
	void SetBombRange(int newRange);
	void AddLife();

	[[nodiscard]] int GetHealth()const { return m_Health; }
	[[nodiscard]] int GetMaxHealth()const { return m_MaxHealth; }


	[[nodiscard]] int GetMaxAmountOfBombs()const { return m_MaxAmountOfBombs + 1; }
	[[nodiscard]] int GetAmountOfBombs()const { return m_AmountOfAvailableBombs; }
	[[nodiscard]] GamepadIndex GetPlayerIndex()const { return m_PlayerIndex; }
	[[nodiscard]] bool GetIsDead()const { return m_IsDead; }
	[[nodiscard]] int GetBaseBombRange() const { return m_BaseBombRange; }
protected:
	//Health member variables
	int const m_StartHealth = 2;
	int const m_MaxHealth = 3;
	int m_Health;
	bool m_IsDead;


	ControllerComponent* m_pController;
	ModelComponent* m_pModel = nullptr;
	float m_Radius, m_Height, m_StepOffset;
	std::vector<int> m_InputKeys{};
	std::vector<int> m_ActionIDs{};
	GamepadIndex const m_PlayerIndex;
	bool m_UseGamePad;

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
	int const m_BaseBombRange = 1;
	int m_AmountOfAvailableBombs = 2;
	int const m_MaxAmountOfBombs = 2;
	std::vector<float>m_BombTimers;
	float const m_BombCooldown = 3.f;
	int m_BombRange;

	//File paths
	std::wstring m_MeshFilePath;
	std::wstring m_MaterialFilePath;

	//Sound
	FMOD::Sound* m_pHitSound = nullptr;
	FMOD::Sound* m_pDeathSound = nullptr;

	//Methods
	[[nodiscard]] DirectX::XMFLOAT3 CalculateBombSpawnPos()const;
	void KillPlayer();
};

