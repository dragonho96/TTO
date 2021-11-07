#include "stdafx.h"
#include "..\Public\ContentBrowser.h"

static const FILESYSTEM::path s_AssetPath = "../../Assets";

CContentBrowser::CContentBrowser(CToolManager * pToolManager)
	: CImGuiWindow(pToolManager), m_FrameCount(0)
{
	
}

void CContentBrowser::Initialize()
{
	m_CurrentDirectory = s_AssetPath;
}


void CContentBrowser::SetContentHierarchy(FILESYSTEM::path curPath)
{

	if (!FILESYSTEM::is_directory(curPath) || curPath.empty())
		return;

	for (auto& iter : FILESYSTEM::directory_iterator(curPath))
	{
		FILESYSTEM::path filePath = iter.path();
		string fileName = iter.path().filename().string();
		if (FILESYSTEM::is_directory(filePath))
		{
			ImGui::PushID(fileName.c_str());
			bool isOpen = ImGui::TreeNode(fileName.c_str());
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				if (FILESYSTEM::is_directory(iter.status()))
					m_CurrentDirectory = filePath;
			}
			if (isOpen)
			{

				ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
				ImGui::TreePop();
				SetContentHierarchy(filePath);
				ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
			}

			ImGui::PopID();
		}
	}
}

void CContentBrowser::Update()
{
	ImGui::Begin("Content Browswer");

	ImGui::Columns(2, "Browser", true);

	if (!m_FrameCount)
	{
		ImGui::SetColumnWidth(0, 180.f);
		++m_FrameCount;
	}

	ImGui::Text("Assets");
	ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
	SetContentHierarchy(s_AssetPath);
	ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

	ImGui::NextColumn();
	if (m_CurrentDirectory != FILESYSTEM::path(s_AssetPath))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
		ImGui::SameLine();
	}
	ImGui::Text(m_CurrentDirectory.string().c_str());

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

		// make every button unique
		ImGui::PushID(fileName.c_str());

		// Needs to load Texture2D
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0, 0, 0, 0 });
		ImGui::ImageButton((ImTextureID)(m_pEngine->GetShaderResourceView()), { thumbnailSize, thumbnailSize }, { 0, 0 }, { 1, 1 });

		//ImGui::Button(fileName.c_str(), { thumbnailSize, thumbnailSize });

		if (ImGui::BeginDragDropSource())
		{
			string pathString = path.string();
			const char* path = pathString.c_str();
			ImGui::SetDragDropPayload("GameObject", path, sizeof(char) * pathString.length() + 1, ImGuiCond_Once);
			ImGui::Text("This is a drag and drop source");
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();
		ImGui::PopID();


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


	ImGui::BeginChild("Thumbnail Size");
	ImGui::Separator();
	ImGui::SliderFloat("", &thumbnailSize, 16, 512);
	ImGui::SameLine();
	ImGui::Text("Thumbnail Size");
	ImGui::EndChild();



	ImGui::EndColumns();
	ImGui::End();
}

void CContentBrowser::LateUpdate()
{
}

