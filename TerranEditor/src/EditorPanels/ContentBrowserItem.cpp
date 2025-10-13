#include "ContentBrowserItem.h"
#include "ContentBrowserPanel.h"

#include "SelectionManager.h"

#include "EditorResources.h"

#include "Core/Input.h"
#include "Project/Project.h"
#include "Utils/Debug/OptickProfiler.h"

#include "UI/UI.h"

#include <imgui_internal.h>

namespace TerranEditor {

#define MAX_RENAME_BUFFER_SIZE 256
static char s_RenameBuffer[MAX_RENAME_BUFFER_SIZE] { 0 };

ContentBrowserItem::ContentBrowserItem(std::string const& name, Terran::Core::UUID const& handle, Terran::Core::Shared<Texture> icon, ItemType type)
    : m_Name(name)
    , m_Icon(icon)
    , m_Handle(handle)
    , m_Type(type)
{
}

void ContentBrowserItem::BeginRender()
{
    ImGui::PushID(&m_Handle);
    ImGui::BeginGroup();
}

void ContentBrowserItem::EndRender()
{
    ImGui::PopID();
}

ItemAction ContentBrowserItem::OnRender()
{
    TR_PROFILE_FUNCTION();
    ItemAction action = ItemAction::None;

    float const cellSize = 74.0f;
    float const edgeOffset = 4.0f;
    int const maxFileNameLength = 8;

    bool isSelected = SelectionManager::IsSelected(SelectionContext::ContentPanel, m_Handle);

    float const textLineHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f + edgeOffset * 2.0f;
    float const infoPanelHeight = std::min(false ? cellSize * 0.5f : textLineHeight, textLineHeight);

    ImVec2 const topLeft = ImGui::GetCursorScreenPos();
    ImVec2 const thumbBottomRight = { topLeft.x + cellSize, topLeft.y + cellSize };
    ImVec2 const infoTopLeft = { topLeft.x, topLeft.y + cellSize };
    ImVec2 const bottomRight = { topLeft.x + cellSize, topLeft.y + cellSize + infoPanelHeight };
    ImRect itemBox = ImRect(topLeft, bottomRight);

    if (ImGui::IsMouseHoveringRect(itemBox.Min, itemBox.Max) || isSelected) {
        auto* drawList = ImGui::GetWindowDrawList();
        ImGuiStyle& style = ImGui::GetStyle();

        drawList->AddRectFilled(topLeft, bottomRight,
            ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Header]), 6.0f);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 4.0f });

    ImGui::InvisibleButton("##thumbnailButton", { cellSize, cellSize });
    UI::ShiftCursor(edgeOffset, -cellSize);
    UI::Image(m_Icon, { cellSize - edgeOffset * 2.0, cellSize - edgeOffset * 2.0 });

    UI::ShiftCursor(edgeOffset, edgeOffset);

    // file name
    {
        ImGui::BeginVertical((std::string("InfoPanel") + m_Name).c_str(), ImVec2(cellSize - edgeOffset * 2.0f, infoPanelHeight - edgeOffset));
        {
            ImGui::BeginHorizontal(m_Name.c_str(), { cellSize - 2.0f, 0.0f });
            ImGui::Spring();
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (cellSize - edgeOffset * 3.0f));
            if (m_IsRenaming) {
                ImGui::SetNextItemWidth(cellSize - edgeOffset * 3.0f);
                ImGui::SetKeyboardFocusHere();
                if (ImGui::InputText("##rename", s_RenameBuffer, MAX_RENAME_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    OnRename(s_RenameBuffer);
                    StopRename();
                }
            } else {
                std::string formattedName = m_Name;
                if (formattedName.size() > maxFileNameLength)
                    formattedName = formattedName.replace(maxFileNameLength, formattedName.size() - 1, "...");
                float const textWidth = std::min(ImGui::CalcTextSize(formattedName.c_str()).x, cellSize);
                ImGui::SetNextItemWidth(textWidth);
                ImGui::Text("%s", formattedName.c_str());
            }
            ImGui::PopTextWrapPos();
            ImGui::Spring();
            ImGui::EndHorizontal();
            ImGui::Spring();
        }
        ImGui::EndVertical();
    }

    ImGui::PopStyleVar();

    UI::ShiftCursor(-edgeOffset, -edgeOffset);

    if (Input::IsKeyPressed(Key::F2) && isSelected && !m_IsRenaming)
        StartRename();

    ImGui::EndGroup();

    if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (m_IsRenaming)
            StopRename();
        else if (SelectionManager::IsSelected(SelectionContext::ContentPanel, m_Handle))
            SelectionManager::Deselect(SelectionContext::ContentPanel, m_Handle);
    }

    if (m_Type == ItemType::Directory) {
        if (ImGui::BeginDragDropTarget()) {
            if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload("ASSET"))
                action = ItemAction::MoveTo;

            ImGui::EndDragDropTarget();
        }
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload("ASSET", m_Handle.GetRaw(), sizeof(Terran::Core::UUID));
        ImGui::Text("%s", m_Name.c_str());
        action = ItemAction::Select;
        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            if (action != ItemAction::MoveTo && !isSelected)
                action = ItemAction::Select;
        }

        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            action = OnActivate();
    }

    if (UI::BeginPopupContextItem("cbi_actions_context_menu")) {
        if (ImGui::MenuItem("Rename"))
            action = ItemAction::StartRename;

        ImGui::EndPopup();
    }

    return action;
}

void ContentBrowserItem::StartRename()
{
    if (m_IsRenaming)
        return;

    memcpy(s_RenameBuffer, m_Name.c_str(), m_Name.size());
    m_IsRenaming = true;
}

void ContentBrowserItem::StopRename()
{
    memset(s_RenameBuffer, 0, MAX_RENAME_BUFFER_SIZE);
    m_IsRenaming = false;
}

ContentBrowserDirectory::ContentBrowserDirectory(Terran::Core::Shared<DirectoryInfo> const& directoryInfo)
    : ContentBrowserItem(directoryInfo->Path.filename().string(), directoryInfo->Handle, EditorResources::DirectoryTexture, ItemType::Directory)
    , m_DirectoryInfo(directoryInfo)
{
}

void ContentBrowserDirectory::Move(std::filesystem::path const& newPath)
{
    auto sourcePath = Project::GetAssetPath() / m_DirectoryInfo->Path;
    auto destinationPath = Project::GetAssetPath() / newPath / m_DirectoryInfo->Path.filename();
    Terran::Core::FileSystem::Rename(sourcePath, destinationPath);
}

ItemAction ContentBrowserDirectory::OnActivate() { return ItemAction::NavigateTo; }
ItemAction ContentBrowserDirectory::OnRename(std::string const& newName)
{
    auto sourcePath = Project::GetAssetPath() / m_DirectoryInfo->Path;
    auto destinationPath = Project::GetAssetPath() / m_DirectoryInfo->Path.parent_path() / newName;
    Terran::Core::FileSystem::Rename(sourcePath, destinationPath);

    return ItemAction::Renamed;
}

ContentBrowserAsset::ContentBrowserAsset(AssetInfo const& assetInfo, Terran::Core::Shared<Texture> const& icon)
    : ContentBrowserItem(assetInfo.Path.stem().string(), assetInfo.Handle, icon, ItemType::File)
    , m_AssetInfo(assetInfo)
{
}

void ContentBrowserAsset::Move(std::filesystem::path const& newPath)
{
    auto sourcePath = Project::GetAssetPath() / m_AssetInfo.Path;
    auto destinationPath = Project::GetAssetPath() / newPath / m_AssetInfo.Path.filename();
    Terran::Core::FileSystem::Rename(sourcePath, destinationPath);
}

ItemAction ContentBrowserAsset::OnActivate() { return ItemAction::Activate; }
ItemAction ContentBrowserAsset::OnRename(std::string const& newName)
{
    auto sourcePath = Project::GetAssetPath() / m_AssetInfo.Path;
    auto destinationPath = Project::GetAssetPath() / m_AssetInfo.Path.parent_path() / (newName + m_AssetInfo.Path.extension().string());
    Terran::Core::FileSystem::Rename(sourcePath, destinationPath);

    return ItemAction::Renamed;
}

}
