#include "Input.h"
#include "WinAPIFramework.h"
#include "Core.h"

KeyState Input::keyState[LAST_KEY_CODE + 1];
Vector2 Input::lastClickMousePos;
int Input::scrollAmount;
std::deque<std::pair<char, KeyState>> Input::KeyEventQueue;
RectF Input::clientRect;

void Input::UpdateKeyState()
{
	std::vector<char> change_to_press_next_frame; // 현재 down인 경우, 별도의 조작이 없다면 다음 프레임엔 press로 변경
	std::vector<char> change_to_none_next_frame; // 현재 up인 경우, 별도의 조작이 없다면 다음 프레임엔 none으로 변경
	while (!KeyEventQueue.empty())
	{
		std::pair<char, KeyState> event = KeyEventQueue.front();
		KeyEventQueue.pop_front();

		if (event.second == KeyState::KEYDOWN)
		{
			switch (keyState[event.first])
			{
			case KeyState::NONE:
				keyState[event.first] = KeyState::KEYDOWN;
				change_to_press_next_frame.push_back(event.first);
				break;
			case KeyState::KEYUP:
				keyState[event.first] = KeyState::KEYDOWN;
				change_to_press_next_frame.push_back(event.first);
				break;
			case KeyState::KEYDOWN:
				break;
			case KeyState::KEYPRESS:
				break;
			}
		}
		else if (event.second == KeyState::KEYUP)
		{
			switch (keyState[event.first])
			{
			case KeyState::NONE:
				break;
			case KeyState::KEYUP:
				break;
			case KeyState::KEYDOWN:
				keyState[event.first] = KeyState::KEYUP;
				change_to_none_next_frame.push_back(event.first);
				break;
			case KeyState::KEYPRESS:
				keyState[event.first] = KeyState::KEYUP;
				change_to_none_next_frame.push_back(event.first);
				break;
			}
		}
		// 다음 두 분기는 윈도우 메시지가 아니라 KeyManager가 이전에 Enter였던 키에 대해 임의로 집어넣은 특별한 경우임
		else if (event.second == KeyState::KEYPRESS)
		{
			switch (keyState[event.first])
			{
			case KeyState::NONE:
				break;
			case KeyState::KEYUP:
				break;
			case KeyState::KEYDOWN:
				keyState[event.first] = KeyState::KEYPRESS;
				break;
			case KeyState::KEYPRESS:
				break;
			}
		}
		else if (event.second == KeyState::NONE)
		{
			switch (keyState[event.first])
			{
			case KeyState::NONE:
				break;
			case KeyState::KEYUP:
				keyState[event.first] = KeyState::NONE;
				break;
			case KeyState::KEYDOWN:
				break;
			case KeyState::KEYPRESS:
				break;
			}
		}
	}

	for (char key : change_to_press_next_frame)
	{
		KeyEventQueue.push_back(std::pair<char, KeyState>(key, KeyState::KEYPRESS));
	}
	for (char key : change_to_none_next_frame)
	{
		KeyEventQueue.push_back(std::pair<char, KeyState>(key, KeyState::NONE));
	}



	// obsolete...
	/*for (int i = 1; i <= LAST_KEY_CODE; i++)
	{
		bool state = ((GetAsyncKeyState(i) & 0x8000) || (GetAsyncKeyState(i) & 0x8001));
		if (state == true)
		{
			if (keyState[i] == KeyState::NONE) keyState[i] = KeyState::KEYDOWN;
			else if (keyState[i] == KeyState::KEYDOWN)
			{
				keyState[i] = KeyState::KEYPRESS;
				lastClickMousePos = GetMousePos();
			}
			else if (keyState[i] == KeyState::KEYUP) keyState[i] = KeyState::KEYDOWN;
			else if (keyState[i] == KeyState::KEYPRESS) keyState[i] = KeyState::KEYPRESS;
		}
		else if (state == false)
		{
			if (keyState[i] == KeyState::NONE) keyState[i] = KeyState::NONE;
			else if (keyState[i] == KeyState::KEYDOWN) keyState[i] = KeyState::KEYUP;
			else if (keyState[i] == KeyState::KEYUP) keyState[i] = KeyState::NONE;
			else if (keyState[i] == KeyState::KEYPRESS) keyState[i] = KeyState::KEYUP;
		}

	}*/

}
void Input::ClearKeyState()
{
	scrollAmount = 0;
}
KeyState Input::GetKeyState(int keyCode)
{
	return keyState[keyCode];
}
KeyState Input::GetKeyState(KeyCode keyCode)
{
	return GetKeyState(static_cast<int>(keyCode));
}
int Input::GetScrollAmount()
{
	return scrollAmount;
}

void Input::SetScrollAmount(int amount)
{
	scrollAmount = amount;
}


bool Input::IsDown(KeyCode key)
{
	return keyState[static_cast<int>(key)] == KeyState::KEYDOWN;
}
bool Input::IsUp(KeyCode key)
{
	return keyState[static_cast<int>(key)] == KeyState::KEYUP;
}
bool Input::IsPress(KeyCode key)
{
	return keyState[static_cast<int>(key)] == KeyState::KEYPRESS;
}



Vector2 Input::GetMousePos()
{
	POINT mousePos;
	GetCursorPos(&mousePos); // 모니터 영역에서 커서의 위치

	HWND hWnd = DG::Core::GetInstance().GetWindowHandle();

	ScreenToClient(hWnd, &mousePos); // 모니터 영역에서의 위치를 현재 창의 위치로 변환 (클라이언트 스크린 좌표)

	// 비트맵만 늘리고 renderer의 clientRect를 갱신하지 않는 경우 때문에 한 번 계산
	RectF bitmapRect = DG::Core::GetInstance().GetScreenRect();
	float widthRatio = bitmapRect.right / clientRect.right;
	float heightRatio = bitmapRect.bottom / clientRect.bottom;

	return Vector2{ (float)mousePos.x * widthRatio, (float)mousePos.y * heightRatio };
}
Vector2 Input::GetLastClickMousePos()
{
	return lastClickMousePos;
}

void Input::UpdateClientRect()
{
	RECT rect;
	GetClientRect(DG::Core::GetInstance().GetWindowHandle(), &rect);
	clientRect = { (float)rect.left, (float)rect.top, (float)rect.right, (float)rect.bottom };

}
