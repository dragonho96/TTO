#include "stdafx.h"
#include "..\Public\ContentBrowser.h"

static const FILESYSTEM::path s_AssetPath = "../../Assets";

CContentBrowser::CContentBrowser(CToolManager * pToolManager)
	: CImGuiWindow(pToolManager)
{
}

void CContentBrowser::Initialize()
{
	m_CurrentDirectory = s_AssetPath;
}

void CContentBrowser::Update()
{

	ImGui::Begin("Content Browswer");




	if (m_CurrentDirectory != FILESYSTEM::path(s_AssetPath))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}

	static float padding = 4.f;
	static float thumbnailSize = 128;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;



	// Saving room for float slider
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild("Content Browser Region", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::Columns(columnCount, 0, false);

	for (auto& iter : FILESYSTEM::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = iter.path();
		string fileName = path.filename().string();
		 
		ImGui::Button(fileName.c_str(), { thumbnailSize, thumbnailSize });

		if (ImGui::BeginDragDropSource())
		{
			string pathString = path.string();
			const char* path = pathString.c_str();
			ImGui::SetDragDropPayload("GameObject", path, sizeof(char) * pathString.length() + 1, ImGuiCond_Once);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (FILESYSTEM::is_directory(iter.status()))
				m_CurrentDirectory /= path.filename();
		}
		
		ImGui::Text(fileName.c_str());

		ImGui::NextColumn();
	}
	ImGui::Columns(1);
	
	ImGui::EndChild();

	ImGui::Separator();
	
	ImGui::BeginChild("Thumbnail Size");
	ImGui::SliderFloat("", &thumbnailSize, 16, 512);
	ImGui::SameLine();
	ImGui::Text("Thumbnail Size");
	ImGui::EndChild();
	



	ImGui::End();

}

void CContentBrowser::LateUpdate()
{
}
