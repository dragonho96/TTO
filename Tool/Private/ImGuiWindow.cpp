#include "stdafx.h"
#include "..\Public\ImGuiWindow.h"

CImGuiWindow::CImGuiWindow(CToolManager * pToolManager)
	: m_pToolManager(pToolManager), 
	m_pEngine(CEngine::GetInstance())
{
}

