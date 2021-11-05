#pragma once
#include "ImGuiWindow.h"
class CContentBrowser : public CImGuiWindow
{
public:
	explicit CContentBrowser(CToolManager* pToolManager);
	virtual ~CContentBrowser() = default;

public:
	// Inherited via CImGuiWindow
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

private:
	FILESYSTEM::path m_CurrentDirectory;
};

