
#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"


Character::Character(float radius, float height, float moveSpeed) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0),
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_MaxRunVelocity(50.0f),
	m_TerminalVelocity(20),
	m_Gravity(9.81f),
	m_RunAccelerationTime(0.3f),
	m_JumpAccelerationTime(0.8f),
	m_RunAcceleration(m_MaxRunVelocity / m_RunAccelerationTime),
	m_JumpAcceleration(m_Gravity / m_JumpAccelerationTime),
	m_RunVelocity(0),
	m_JumpVelocity(0),
	m_Velocity(0, 0, 0)
{}

void Character::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	auto const physx = PhysxManager::GetInstance()->GetPhysics();
	auto* pControllerComponent = new ControllerComponent(physx->createMaterial(0.f, 0.f, 0.f), m_Radius, m_Height);
	m_pController = pControllerComponent;
	AddComponent(pControllerComponent);


	auto* pFixedCamera = new CameraComponent();
	m_pCamera = pFixedCamera;
	AddComponent(pFixedCamera);


	auto inputAction = InputAction(CharacterMovement::LEFT, InputTriggerState::Down, VK_LEFT);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::RIGHT, InputTriggerState::Down, VK_RIGHT);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::BACKWARD, InputTriggerState::Down, VK_DOWN);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::FORWARD, InputTriggerState::Down, VK_UP);
	gameContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(CharacterMovement::JUMP, InputTriggerState::Pressed, VK_SPACE);
	gameContext.pInput->AddInputAction(inputAction);
}

void Character::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
	m_pCamera->SetActive();
}

void Character::Update(const GameContext& gameContext)
{
	using namespace DirectX;

	//***********
	//CAMERA
	//***********

	//Camera input
	auto move = XMFLOAT2(0, 0);
	if (gameContext.pInput->IsActionTriggered(CharacterMovement::LEFT))
		move.x = -1.f;
	else if (gameContext.pInput->IsActionTriggered(CharacterMovement::RIGHT))
		move.x = 1.f;

	if (gameContext.pInput->IsActionTriggered(CharacterMovement::FORWARD))
		move.y = 1.f;
	else if (gameContext.pInput->IsActionTriggered(CharacterMovement::BACKWARD))
		move.y = -1.f;

	//Camera transforms
	const auto forward = XMLoadFloat3(&GetTransform()->GetForward());
	const auto right = XMLoadFloat3(&GetTransform()->GetRight());
	auto currPos = XMLoadFloat3(&GetTransform()->GetPosition());

	auto look = XMFLOAT2(0, 0);
	const auto mouseMove = InputManager::GetMouseMovement();
	look.x = static_cast<float>(mouseMove.x);
	look.y = static_cast<float>(mouseMove.y);

	currPos += forward * move.y * m_MoveSpeed * gameContext.pGameTime->GetElapsed();
	currPos += right * move.x * m_MoveSpeed * gameContext.pGameTime->GetElapsed();

	m_TotalYaw += look.x * m_RotationSpeed * gameContext.pGameTime->GetElapsed();
	m_TotalPitch += look.y * m_RotationSpeed * gameContext.pGameTime->GetElapsed();


	m_pCamera->GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);
	//m_pCamera->GetTransform()->Translate(currPos);

	//***********
	//CONTROLLER
	//***********

	//Determine moveDirection
	auto const cameraForward = GetTransform()->GetForward();
	auto const cameraRight = GetTransform()->GetRight();
	auto forwardVector = XMLoadFloat3(&cameraForward);
	auto rightVector = XMLoadFloat3(&cameraRight);
	auto directionVector = move.x * rightVector + move.y * forwardVector;

	auto moveV = XMFLOAT3(move.x, 0, move.y);

	XMFLOAT3 moveDirection{};
	XMStoreFloat3(&moveDirection, directionVector);


	//If x or z movement is different from 0, calculate run velocity
	if (moveV.x != 0 || moveV.z != 0)
	{
		m_RunVelocity += m_RunAcceleration * gameContext.pGameTime->GetElapsed();
		if (m_RunVelocity > m_MaxRunVelocity)
			m_RunVelocity = m_MaxRunVelocity;

		m_Velocity.x = moveDirection.x * m_RunVelocity;
		m_Velocity.z = moveDirection.z * m_RunVelocity;
	}
	else
	{
		m_Velocity.x = 0;
		m_Velocity.z = 0;
	}

	//Apply gravity
	if (!m_pController->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_JumpVelocity -= m_Gravity * gameContext.pGameTime->GetElapsed();
		Clamp(m_JumpVelocity, m_TerminalVelocity, -m_TerminalVelocity);

	}
	else if (gameContext.pInput->IsActionTriggered(CharacterMovement::JUMP))
	{
		m_JumpVelocity = 0;
		m_Velocity.y = 200.f;
	}
	else
		m_Velocity.y = 0;

	m_Velocity.y += m_JumpVelocity;

	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);
	velocity *= gameContext.pGameTime->GetElapsed();

	XMFLOAT3 moveVelocity{};
	XMStoreFloat3(&moveVelocity, velocity);
	m_pController->Move(moveVelocity);
}