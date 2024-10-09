#pragma once
#include <deque>
#include "Vector2.h"
#include "Structures.h"

// �����Ӹ��� Ű�� �����ؾ� �ϴ� ����
// 1. Ű �Է� ���� ����ȭ
// 2. ������ �� ���� ����ȭ (�������� �������� �� Ű ���°� ����Ǿ �� ��)

// DirectInput�� ���������� ������ �޽����� ó����. �� �� ������尡 �߻��ϱ� ������ �����е� �Է� ó�������θ� �� ���� ������. (���콺, Ű������ ��� ������ �޽����� ���� ó���ϴ� ���� ����.)
// GetAsyncKeyState�� â�� ��Ŀ�� ���ο� �����ϰ� �������� �Է� ���¸� �������Ƿ� ��뵵 ũ��, �ٸ� �ۿ� ���� ��ŷ�� ���ɼ��� ����. MSDN�� WM_KEYUP, WM_KEYDOWN�� �� ���� �ǰ���.

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
	static RectF clientRect; // ��Ʈ�ʸ� �ø��� rect�� �������� �ʴ� ��찡 �־ Input�� ���� ����
public:
	static std::deque<std::pair<char, KeyState>> KeyEventQueue;

	static void UpdateKeyState();
	static void ClearKeyState(); // ��ũ�� ������ �ʿ�
	static KeyState GetKeyState(int keyCode);
	static KeyState GetKeyState(KeyCode keyCode);
	static int GetScrollAmount();
	static void SetScrollAmount(int amount);

	static bool IsDown(KeyCode);
	static bool IsUp(KeyCode);
	static bool IsPress(KeyCode);

	// ���콺�� ��ũ�� ��ǥ�� ��ȯ�մϴ�.
	static Vector2 GetMousePos();
	static Vector2 GetLastClickMousePos();

	static void UpdateClientRect();
};

