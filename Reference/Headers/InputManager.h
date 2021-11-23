#pragma once

#include "Base.h"

#define KEY_MAX 256
#define MAX_INPUT_MOUSE 8

BEGIN(Engine)
class CInputManager : public CBase
{
	DECLARE_SINGLETON(CInputManager)

private:
	explicit CInputManager();
	virtual ~CInputManager() = default;
	virtual void Free() override;

public:
	void Initialize();
	void Update();
public:
	void InputProc(const HWND hWnd, const UINT message, 
		const WPARAM wParam, const LPARAM lParam);
public:
	bool IsKeyDown(_uint key);
	bool IsKeyUp(_uint key);
	bool IsKeyPressed(_uint key);
	bool IsKeyToggled(_uint Key);
public:
	bool IsMouseDown(DWORD mouse);
	bool IsMouseUp(DWORD mouse);
	bool IsMousePressed(DWORD mouse);

public:
	const _float3 GetMouseMoveValue() const;
	const _float3 GetMousePosition() const;

private:
	bitset<KEY_MAX>		m_keyUp;
	bitset<KEY_MAX>		m_keyDown;

	HWND	m_hWnd;
	_float3 position; //마우스 위치

	byte buttonStatus[MAX_INPUT_MOUSE];
	byte buttonOldStatus[MAX_INPUT_MOUSE];
	byte buttonMap[MAX_INPUT_MOUSE];

	_float3 wheelStatus;
	_float3 wheelOldStatus;
	_float3 wheelMoveValue;

	DWORD timeDblClk;
	DWORD startDblClk[MAX_INPUT_MOUSE];
	int buttonCount[MAX_INPUT_MOUSE];

	enum
	{
		MOUSE_ROTATION_NONE = 0,
		MOUSE_ROTATION_LEFT,
		MOUSE_ROTATION_RIGHT
	};

	enum
	{
		BUTTON_INPUT_STATUS_NONE = 0,
		BUTTON_INPUT_STATUS_DOWN,
		BUTTON_INPUT_STATUS_UP,
		BUTTON_INPUT_STATUS_PRESS,
		BUTTON_INPUT_STATUS_DBLCLK
	};
};

END