#pragma once
#include <deque>
#include "Vector2.h"
#include "Structures.h"

// 프레임마다 키를 갱신해야 하는 이유
// 1. 키 입력 감지 간편화
// 2. 프레임 내 상태 동기화 (프레임이 진행중일 땐 키 상태가 변경되어선 안 됨)

// DirectInput은 내부적으로 윈도우 메시지를 처리함. 이 때 오버헤드가 발생하기 때문에 게임패드 입력 처리용으로만 쓸 것을 권장함. (마우스, 키보드의 경우 윈도우 메시지를 직접 처리하는 것이 좋음.)
// GetAsyncKeyState는 창의 포커싱 여부와 무관하게 윈도우의 입력 상태를 가져오므로 비용도 크고, 다른 앱에 의해 후킹될 가능성이 있음. MSDN은 WM_KEYUP, WM_KEYDOWN을 쓸 것을 권고함.

#define LAST_KEY_CODE 255
enum class KeyState { NONE, KEYUP, KEYDOWN, KEYPRESS };
enum class KeyCode
{
	MouseLeft = 0x01,
	MouseRight = 0x02,
	MouseCancle = 0x03,
	MouseMiddle = 0x04,
	Backspace = 0x08,
	Tab = 0x09,
	Clear = 0x0C,
	Enter = 0x0D,
	Shift = 0x10,
	Ctrl = 0x11,
	Alt = 0x12,
	Pause = 0x13,
	CapsLock = 0x14,
	Esc = 0x1B,
	Space = 0x20,
	PageUp = 0x21,
	PageDown = 0x22,
	End = 0x23,
	Home = 0x24,
	ArrowLeft = 0x25,
	ArrowUp = 0x26,
	ArrowRight = 0x27,
	ArrowDown = 0x28,
	Print = 0x2A,
	Insert = 0x2D,
	Delete = 0x2E,
	Num0 = 0x30,
	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,
	A = 0x41,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	WindowLeft = 0x5B,
	WindowRight = 0x5C,
	Numpad0 = 0x60,
	Numpad1,
	Numpad2,
	Numpad3,
	Numpad4,
	Numpad5,
	Numpad6,
	Numpad7,
	Numpad8,
	Numpad9,
	F1 = 0x70,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	NumLock = 0x90,
	ScrollLock = 0x91,
	ShiftLeft = 0xA0,
	ShiftRight = 0xA1,
	CtrlLeft = 0xA2,
	CtrlRight = 0xA3,
	AltLeft = 0xA4,
	AltRight = 0xA5
};

class Input
{
	static KeyState keyState[LAST_KEY_CODE + 1];
	static Vector2 lastClickMousePos;
	static int scrollAmount;
	static RectF clientRect; // 비트맵만 늘리고 rect는 갱신하지 않는 경우가 있어서 Input만 따로 관리
public:
	static std::deque<std::pair<char, KeyState>> KeyEventQueue;

	static void UpdateKeyState();
	static void ClearKeyState(); // 스크롤 때문에 필요
	static KeyState GetKeyState(int keyCode);
	static KeyState GetKeyState(KeyCode keyCode);
	static int GetScrollAmount();
	static void SetScrollAmount(int amount);

	static bool IsDown(KeyCode);
	static bool IsUp(KeyCode);
	static bool IsPress(KeyCode);

	// 마우스의 스크린 좌표를 반환합니다.
	static Vector2 GetMousePos();
	static Vector2 GetLastClickMousePos();

	static void UpdateClientRect();
};

