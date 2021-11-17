#pragma once
#include "ImGuiWindow.h"
#include "Texture.h"

BEGIN(Tool)
class CContentBrowser : public CImGuiWindow
{
public:
	explicit CContentBrowser();
	virtual ~CContentBrowser() = default;
	virtual void Free() override;

public:
	// Inherited via CImGuiWindow
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void SetContentHierarchy(FILESYSTEM::path curPath);

private:
	FILESYSTEM::path	m_CurrentDirectory;
	_uint				m_FrameCount;
	CTexture*			m_pTexFolder;
	CTexture*			m_pTexFile;
	CTexture*			m_pTexImage;
	// Inherited via CImGuiWindow

};

END