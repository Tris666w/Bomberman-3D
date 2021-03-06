#pragma once
#include <map>

enum class GamepadIndex : DWORD
{
	PlayerOne = 0,
	PlayerTwo = 1,
	PlayerThree = 2,
	PlayerFour = 3
};

enum class InputTriggerState
{
	Pressed,
	Released,
	Down
};

enum class GamePadKeys
{
	DPAD_UP = 0x0001,
	DPAD_DOWN = 0x0002,
	DPAD_LEFT = 0x0004,
	DPAD_RIGHT = 0x0008,
	START = 0x0010,
	BACK = 0x0020,
	LEFT_THUMB = 0x0040,
	RIGHT_THUMB = 0x0080,
	LEFT_SHOULDER = 0x0100,
	RIGHT_SHOULDER = 0x0200,
	A = 0x1000,
	B = 0x2000,
	X = 0x4000,
	Y = 0x8000
};
struct InputAction
{
	InputAction() :
		ActionID(-1),
		TriggerState(InputTriggerState::Pressed),
		KeyboardCode(-1),
		MouseButtonCode(-1),
		GamepadButtonCode(0),
		PlayerIndex(GamepadIndex::PlayerOne),
		IsTriggered(false) {}

	InputAction(int actionID, InputTriggerState triggerState = InputTriggerState::Pressed, int keyboardCode = -1, int mouseButtonCode = -1, WORD gamepadButtonCode = 0, GamepadIndex playerIndex = GamepadIndex::PlayerOne) :
		ActionID(actionID),
		TriggerState(triggerState),
		KeyboardCode(keyboardCode),
		MouseButtonCode(mouseButtonCode),
		GamepadButtonCode(gamepadButtonCode),
		PlayerIndex(playerIndex),
		IsTriggered(false) {}

	int ActionID;
	InputTriggerState TriggerState;
	int KeyboardCode; //VK_... (Range 0x07 <> 0xFE)
	int MouseButtonCode; //VK_... (Range 0x00 <> 0x06)
	WORD GamepadButtonCode; //XINPUT_GAMEPAD_...
	GamepadIndex PlayerIndex;
	bool IsTriggered;
};

class InputManager
{
public:
	InputManager(const InputManager& other) = delete;
	InputManager(InputManager&& other) noexcept = delete;
	InputManager& operator=(const InputManager& other) = delete;
	InputManager& operator=(InputManager&& other) noexcept = delete;
	InputManager() = default;
	~InputManager();

	static void Initialize();
	void Update();
	bool AddInputAction(InputAction action);
	bool IsActionTriggered(int actionID);
	[[nodiscard]] int GetAvailableActionID()const;
	static void ForceMouseToCenter(bool force);
	static void SetEnabled(bool enabled) { m_Enabled = enabled; }

	static POINT GetMousePosition(bool previousFrame = false) { return (previousFrame) ? m_OldMousePosition : m_CurrMousePosition; }
	static POINT GetMouseMovement() { return m_MouseMovement; }

	static DirectX::XMFLOAT2 GetThumbstickPosition(bool leftThumbstick = true, GamepadIndex playerIndex = GamepadIndex::PlayerOne);
	static float GetTriggerPressure(bool leftTrigger = true, GamepadIndex playerIndex = GamepadIndex::PlayerOne);
	static void SetVibration(float leftVibration, float rightVibration, GamepadIndex playerIndex = GamepadIndex::PlayerOne);

	static void CursorVisible(bool visible) { ShowCursor(visible); }
	static bool IsGamepadConnected(const GamepadIndex index) { return m_ConnectedGamepads[static_cast<DWORD>(index)]; }

	static bool IsKeyboardKeyDown(int key, bool previousFrame = false);
	static bool IsMouseButtonDown(int button, bool previousFrame = false);
	static bool IsGamepadButtonDown(WORD button, GamepadIndex playerIndex = GamepadIndex::PlayerOne, bool previousFrame = false);
	static void RefreshControllerConnections();

private:

	std::map<int, InputAction> m_InputActions;

	static BYTE* m_pCurrKeyboardState, * m_pOldKeyboardState, * m_pKeyboardState0, * m_pKeyboardState1;
	static bool m_KeyboardState0Active;
	static POINT m_CurrMousePosition, m_OldMousePosition, m_MouseMovement;
	static bool m_IsInitialized;

	static XINPUT_STATE m_OldGamepadState[XUSER_MAX_COUNT], m_CurrGamepadState[XUSER_MAX_COUNT];
	static bool m_ConnectedGamepads[XUSER_MAX_COUNT];
	static bool m_Enabled;
	static bool m_ForceToCenter;

	static void UpdateGamepadStates();
	static bool UpdateKeyboardStates();

	static bool IsKeyboardKeyDown_unsafe(int key, bool previousFrame = false);
	static bool IsMouseButtonDown_unsafe(int button, bool previousFrame = false);
	static bool IsGamepadButtonDown_unsafe(WORD button, GamepadIndex playerIndex = GamepadIndex::PlayerOne, bool previousFrame = false);
};