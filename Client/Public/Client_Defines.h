#pragma once

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;

enum SCENE { SCENE_STATIC, SCENE_LOGO, SCENE_LOADING, SCENE_GAMEPLAY, SCENE_TEST, SCENE_END };

extern HWND		g_hWnd;
extern BOOL		g_Done;
namespace Client {}
using namespace Client;

#define NO_EVENT 0
#define CHANGE	 0
#define WINCX	1280
#define WINCY	720

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#define	ADDLOG(log)																	\
	if (CEngine::GetInstance()->GetWindow("Log"))									\
	{																				\
		dynamic_cast<CLog*>(CEngine::GetInstance()->GetWindow("Log"))->AddLog(log); \
	}																				
