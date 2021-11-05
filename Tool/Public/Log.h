#pragma once
#include "ImGuiWindow.h"
class CLog : public CImGuiWindow
{
public:
	explicit CLog(CToolManager* pToolManager);
	virtual ~CLog() = default;

public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

public:
	void	ClearLog();
	void    ExecCommand(const char* command_line);
	void    AddLog(const char* fmt, ...);

private:
	char                  InputBuf[256];
	ImVector<char*>       Items;
	ImVector<const char*> Commands;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter       Filter;
	bool                  AutoScroll;
	bool                  ScrollToBottom;
};
