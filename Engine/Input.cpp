
#include "pch.h"
#include "Input.h"
#include "Engine.h"



void Input::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);  //�ƹ��͵� �ȴ��� ����
}

void Input::Update()  //�� �����Ӹ��� ȣ���Ұ���.
{
	HWND hwnd = ::GetActiveWindow();  //���� ������������� â�� �ƴ϶�� ������.
	if (_hwnd != hwnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
			_states[key] = KEY_STATE::NONE;  //���� ĳ��

		return;
	}
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;


	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// Ű�� ���� ������ true
		if (asciiKeys[key] & 0x80)
		{
			KEY_STATE& state = _states[key];

			// ���� �����ӿ� Ű�� ���� ���¶�� PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = _states[key];

			// ���� �����ӿ� Ű�� ���� ���¶�� UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	//���� â���� ���콺 ��ġ�� ������ �� API
	::GetCursorPos(&_mousePos);
	::ScreenToClient(GEngine->GetWindow().hwnd, &_mousePos);

}