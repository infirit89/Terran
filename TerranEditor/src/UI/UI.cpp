#include "UI.h"

#include "Terran.h"
#include "Core/Log.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace TerranEditor 
{
	using namespace TerranEngine;
	bool UI::DrawColor4Control(const std::string& label, glm::vec4& value, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());
		
		{
			const ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable colorControlTable(label, tableInfo);
			const ImVec2 buttonSize = ImVec2{ ImGui::CalcItemWidth(), 0.0f };

			if (ImGui::ColorButton("##colbutton", ImVec4{ value.r, value.g, value.b, value.a }, 0, buttonSize))
				ImGui::OpenPopup("picker");

			if (ImGui::BeginPopup("picker"))
			{
				changed = ImGui::ColorPicker4("##colpicker", glm::value_ptr(value));
				ImGui::EndPopup();
			}
		}
		
		ImGui::PopID();

		return changed;
	}

	bool UI::DrawVec2Control(const std::string& label, glm::vec2& value, float power, const char* format, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			const ImGuiIO io = ImGui::GetIO();
			
			ScopedVarTable::TableInfo tableInfo;
			tableInfo.itemCount = 2;
			ScopedVarTable vec2Table(label, tableInfo);

			ScopedStyleVar itemSpacingStyleVar({
				{ ImGuiStyleVar_ItemSpacing, { 2, GImGui->Style.ItemSpacing.y } }
				});

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight, lineHeight };
		
			ScopedStyleColor buttonStyleColor({
				{ ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } }
				});

			ImGui::Button("X", buttonSize);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				value.x += io.MouseDelta.x * power;
			}

			ImGui::SameLine();

			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosX(cursorPos.x - 4.5f);
			if (ImGui::DragFloat("##DRX", &value.x, power, 0.0f, 0.0f, format))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

			ImGui::PopItemWidth();

			ImGui::SameLine();

			ImGui::Button("Y", buttonSize);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				value.y += io.MouseDelta.x * power;
			}

			ImGui::SameLine();

			cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosX(cursorPos.x - 4.5f);
			if (ImGui::DragFloat("##DRY", &value.y, power, 0.0f, 0.0f, format))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

			ImGui::PopItemWidth();

		}

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool UI::DrawEntityControl(const std::string& label, UUID& value, const Shared<Scene>& scene, float columnWidth)
	{
		ScopedVarTable::TableInfo tableInfo;
		ScopedVarTable textureTable(label, tableInfo);

		Entity entity = scene->FindEntityWithUUID(value);
		
		char buf[256];
		memset(buf, 0, sizeof(buf));

		strcpy_s(buf, sizeof(buf), !entity ? "None" : entity.GetName().c_str());
						
		ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_DontRenderCursor | ImGuiInputTextFlags_DontChangeMouseCursorOnHover;
		ImGui::InputText("##TextureField", buf, sizeof(buf), inputTextFlags);

		if (ImGui::BeginDragDropTarget()) 
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_UUID")) 
			{
				TR_ASSERT(payload->DataSize == 16 * sizeof(uint8_t), "The Drag/Drop Payload data's size doesn't match the required size");
				std::array<uint8_t, 16> idArr = { 0 };
				memcpy(idArr._Elems, payload->Data, 16 * sizeof(uint8_t));
				UUID id(idArr);
				Entity receivedEntity = scene->FindEntityWithUUID(id);

				if(receivedEntity)
				{
					value = receivedEntity.GetID();
					return true;
				}
			}

			ImGui::EndDragDropTarget();
		}

		return false;
	}

	static std::string ProccessFieldName(std::string name)
	{
		std::string result;

		name[0] = toupper(name[0]);

		for (size_t i = 0; i < name.size(); i++)
		{
			if (isupper(name.at(i)))
			{
				result += " ";
				result += name.at(i);
			}
			else
				result += name.at(i);
		}

		return result;
	}

	void UI::DrawScriptField(const TerranEngine::Shared<Scene>& scene, TerranEngine::ScriptField* field, const TerranEngine::GCHandle& handle)
	{
		TR_ASSERT(handle.IsValid(), "Invalid handle");

		std::string fieldName = ProccessFieldName(field->GetName());
		switch (field->GetType().TypeEnum)
		{
		case ScriptType::Bool:
		{
			bool value = field->GetData<bool>(handle);

			if (UI::DrawBoolControl(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::Char:
		{
			char value = (char)field->GetData<wchar_t>(handle);
			// TODO: kinda hacky implementation, make a UI::DrawCharControl function
			std::string strVal; strVal += value;
			if (UI::DrawStringControl(fieldName, strVal, 0, 2))
			{
				if (strVal.empty())
					break;
				const wchar_t wc = strVal.at(0);
				field->SetData<wchar_t>(wc, handle);
			}

			break;
		}
		case ScriptType::Int8:
		{
			int8_t value = field->GetData<int8_t>(handle);

			if (UI::DrawScalar(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::Int16:
		{
			int16_t value = field->GetData<int16_t>(handle);

			if (UI::DrawScalar(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::Int32:
		{
			int32_t value = field->GetData<int32_t>(handle);
			
			if (!field->GetType().IsEnum()) 
			{
				if (UI::DrawScalar(fieldName, value))
					field->SetData(value, handle);
			}
			else 
			{
				ScriptClass* klass = field->GetType().GetTypeClass();
				std::vector<ScriptField> enumFields = klass->GetEnumFields();

				const char** enumFieldNames = new const char*[enumFields.size()];

				for (size_t i = 0; i < enumFields.size(); i++)
					enumFieldNames[i] = enumFields[i].GetName();

				if (UI::DrawComboBox(field->GetName(), enumFieldNames, enumFields.size(), value))
					field->SetData(value, handle);

				delete[] enumFieldNames;
			}


			break;
		}
		case ScriptType::Int64:
		{
			int64_t value = field->GetData<int64_t>(handle);

			if (UI::DrawScalar(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::UInt8:
		{
			uint8_t value = field->GetData<uint8_t>(handle);

			if (UI::DrawScalar(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::UInt16:
		{
			uint16_t value = field->GetData<uint16_t>(handle);

			if (UI::DrawScalar(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::UInt32:
		{
			uint32_t value = field->GetData<uint32_t>(handle);

			if (UI::DrawScalar(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::UInt64:
		{
			uint64_t value = field->GetData<uint64_t>(handle);

			if (UI::DrawScalar(fieldName, value))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::Float:
		{
			float value = field->GetData<float>(handle);

			if (UI::DrawFloatControl(fieldName, value, 0.1f, "%.2f"))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::Double:
		{
			double value = field->GetData<double>(handle);

			if (UI::DrawScalar(fieldName, value, 0.1f, "%.4f"))
				field->SetData(value, handle);

			break;
		}
		case ScriptType::String:
		{
			std::string value = field->GetData<std::string>(handle);

			if (UI::DrawStringControl(fieldName, value))
				field->SetData<const char*>(value.c_str(), handle);

			break;
		}
		case ScriptType::Vector2:
		{
			glm::vec2 value = field->GetData<glm::vec2>(handle);

			if (UI::DrawVec2Control(fieldName, value))
				field->SetData<glm::vec2>(value, handle);

			break;
		}
		case ScriptType::Vector3:
		{
			glm::vec3 value = field->GetData<glm::vec3>(handle);

			if (UI::DrawVec3Control(fieldName, value))
				field->SetData<glm::vec3>(value, handle);

			break;
		}
		case ScriptType::Color:
		{
			glm::vec4 value = field->GetData<glm::vec4>(handle);

			if (UI::DrawColor4Control(fieldName, value))
				field->SetData<glm::vec4>(value, handle);

			break;
		}
		case ScriptType::Entity:
		{
			UUID value = field->GetData<UUID>(handle);
			if (UI::DrawEntityControl(fieldName, value, scene))
				field->SetData<UUID>(value, handle);
			break;
		}
		}
	}

	bool UI::DrawScriptArrayField(const Shared<Scene>& scene, const std::string& fieldName, ScriptArray& array)
	{
		bool hasChanged = false;
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_AllowItemOverlap;

		ImGui::Unindent(20.0f);
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

		bool opened = ImGui::TreeNodeEx(fieldName.c_str(), flags);

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 1.5f);

		ImGui::PushItemWidth(lineHeight * 1.5f);
		uint32_t arrayLength = array.Length();
		if (ImGui::DragScalar("##array_size", ImGuiDataType_U32, &arrayLength, 0.1f, nullptr, nullptr, nullptr))
		{
			array.Resize(arrayLength);
			hasChanged = true;
		}
		ImGui::PopItemWidth();

		ImGuiTreeNodeFlags elementsFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_NoButton | ImGuiTreeNodeFlags_FramePadding;

		if (opened)
		{
			for (uint32_t i = 0; i < array.Length(); i++)
			{
				std::string elementName = std::to_string(i);
				switch (array.GetType().TypeEnum)
				{
				case ScriptType::Bool:
				{
					bool value = array.Get<bool>(i);
					if (UI::DrawBoolControl(elementName, value))
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Char:
				{
					char value = (char)array.Get<wchar_t>(i);
					// TODO: kinda hacky implementation, make a UI::DrawCharControl function
					std::string strVal; strVal += value;
					if (UI::DrawStringControl(elementName, strVal, 0, 2))
					{
						if (strVal.empty())
							break;
						const wchar_t wc = strVal.at(0);
						array.Set<wchar_t>(i, wc);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Int8:
				{
					int8_t value = array.Get<int8_t>(i);

					if (UI::DrawScalar(elementName, value)) 
					{
						array.Set(i, value); 
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Int16:
				{
					int16_t value = array.Get<int16_t>(i);

					if (UI::DrawScalar(elementName, value)) 
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Int32:
				{
					int32_t value = array.Get<int32_t>(i);

					if (UI::DrawScalar(elementName, value))
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Int64:
				{
					int64_t value = array.Get<int64_t>(i);

					if (UI::DrawScalar(elementName, value)) 
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::UInt8:
				{
					uint8_t value = array.Get<uint8_t>(i);

					if (UI::DrawScalar(elementName, value))
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::UInt16:
				{
					uint16_t value = array.Get<uint16_t>(i);

					if (UI::DrawScalar(elementName, value)) 
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::UInt32:
				{
					uint32_t value = array.Get<uint32_t>(i);

					if (UI::DrawScalar(elementName, value)) 
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::UInt64:
				{
					uint64_t value = array.Get<uint64_t>(i);

					if (UI::DrawScalar(elementName, value)) 
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Float:
				{
					float value = array.Get<float>(i);

					if (UI::DrawFloatControl(elementName, value, 0.1f, "%.2f")) 
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Double:
				{
					double value = array.Get<double>(i);

					if (UI::DrawScalar(elementName, value, 0.1f, "%.4f")) 
					{
						array.Set(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::String:
				{
					std::string value = array.At(i);

					if (UI::DrawStringControl(fieldName, value)) 
					{
						array.Set<Utils::Variant>(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Vector2:
				{
					glm::vec2 value = array.Get<glm::vec2>(i);

					if (UI::DrawVec2Control(elementName, value)) 
					{
						array.Set<glm::vec2>(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Vector3:
				{
					glm::vec3 value = array.Get<glm::vec3>(i);

					if (UI::DrawVec3Control(elementName, value)) 
					{
						array.Set<glm::vec3>(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Color:
				{
					glm::vec4 value = array.Get<glm::vec4>(i);

					if (UI::DrawColor4Control(elementName, value)) 
					{
						array.Set<glm::vec4>(i, value);
						hasChanged = true;
					}

					break;
				}
				case ScriptType::Entity:
				{
					UUID value = array.Get<UUID>(i);
					if (UI::DrawEntityControl(elementName, value, scene)) 
					{
						array.Set<UUID>(i, value);
						hasChanged = true;
					}
					break;
				}
				}
			}

			ImGui::TreePop();
		}
		ImGui::Indent(20.0f);

		return hasChanged;
	}

	bool UI::DrawVec3Control(const std::string& label, glm::vec3& value, float power, const char* format, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			const ImGuiIO io = ImGui::GetIO();
			
			ScopedVarTable::TableInfo tableInfo;
			tableInfo.itemCount = 3;
			ScopedVarTable vec3Table(label, tableInfo);

			ScopedStyleVar itemSpacingStyleVar({
			   { ImGuiStyleVar_ItemSpacing, { 2, GImGui->Style.ItemSpacing.y } }
				});

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight, lineHeight };

			ScopedStyleColor buttonStyleColor({
				{ ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } }
				});

			ImGui::Button("X", buttonSize);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) 
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				value.x += io.MouseDelta.x * power;
			}

			ImGui::SameLine();

			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosX(cursorPos.x - 4.5f);
			if (ImGui::DragFloat("##DRX", &value.x, power, 0.0f, 0.0f, format))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::Button("Y", buttonSize);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) 
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				value.y += io.MouseDelta.x * power;
			}

			ImGui::SameLine();

			cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosX(cursorPos.x - 4.5f);
			if (ImGui::DragFloat("##DRY", &value.y, power, 0.0f, 0.0f, format))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::Button("Z", buttonSize);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) 
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				value.z += io.MouseDelta.x * power;
			}

			ImGui::SameLine();

			cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosX(cursorPos.x - 4.5f);
			if (ImGui::DragFloat("##DRZ", &value.z, power, 0.0f, 0.0f, format))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

			ImGui::PopItemWidth();
		}
		
		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool UI::DrawFloatControl(const std::string& label, float& value, float power, const char* format, float columnWidth)
	{
		bool changed = false;

		ImGui::PushID(label.c_str());
		
		{
			ScopedVarTable::TableInfo tableInfo;

			ScopedVarTable floatTable(label, tableInfo);

			if (ImGui::DragFloat("##val", &value, power, 0.0f, 0.0f, format))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		}

		ImGui::PopID();

		return changed;
	}

	bool UI::DrawIntControl(const std::string& label, int& value, float power, float columnWidth)
	{
		bool changed = false;

		ImGui::PushID(label.c_str());

		{
			ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable intTable(label, tableInfo);

			if (ImGui::DragInt("##val", &value, power))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		}
		
		ImGui::PopID();

		return changed;
	}

	bool UI::DrawBoolControl(const std::string& label, bool& value, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable floatTable(label, tableInfo);
			changed = ImGui::Checkbox("##val", &value);
		}

		ImGui::PopID();

		return changed;
	}

	bool UI::DrawStringControl(const std::string& label, std::string& value, ImGuiInputTextFlags flags, int maxBufSize, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			const ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable floatTable(label, tableInfo);

			char* buf = new char[maxBufSize];
			memset(buf, 0, maxBufSize);
			strcpy_s(buf, maxBufSize, value.c_str());

			if (ImGui::InputText("##val", buf, maxBufSize, flags))
			{
				value = buf;
				changed = true;
			}
		}

		ImGui::PopID();

		return changed;
	}

	UI::ScopedVarTable::ScopedVarTable(const std::string& name, TableInfo tableInfo)
		: m_TableInfo(tableInfo)
	{
		// NOTE: this is a very fucking bad
		char tableID[256];

		tableID[0] = '#';
		strcpy_s(tableID + 1, sizeof(tableID) - 1, name.c_str());
		tableID[strlen(name.c_str()) + 1] = '\0';

		ImGui::Columns(tableInfo.columnCount, tableID, tableInfo.border);
		ImGui::SetColumnWidth(0, tableInfo.firstColumnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		if (tableInfo.itemCount > 1)
			ImGui::PushMultiItemsWidths(tableInfo.itemCount, ImGui::CalcItemWidth());
		else
			ImGui::PushItemWidth(ImGui::CalcItemWidth());

	}

	UI::ScopedVarTable::~ScopedVarTable()
	{
		if (m_TableInfo.itemCount == 1)
			ImGui::PopItemWidth();

		ImGui::Columns(1);

		//delete[] m_TableID;
	}

	UI::ScopedStyleColor::ScopedStyleColor(std::initializer_list<StyleColor> styleColorList)
		: m_StyleColorListSize(styleColorList.size())
	{
		for (auto& styleColor : styleColorList)
		{
			ImVec4 color = { styleColor.Color.x, styleColor.Color.y, styleColor.Color.z, styleColor.Color.w };
			ImGui::PushStyleColor(styleColor.ColorVarIdx, color);
		}
	}

	UI::ScopedStyleColor::~ScopedStyleColor() { ImGui::PopStyleColor((int)m_StyleColorListSize); }

	UI::ScopedStyleVar::ScopedStyleVar(std::initializer_list<StyleVar> styleVarList)
		: m_StyleVarListSize(styleVarList.size())
	{
		for (auto& styleVarIt : styleVarList)
		{
			ImVec2 styleVal = { styleVarIt.Val.x, styleVarIt.Val.y };
			ImGui::PushStyleVar(styleVarIt.StyleVarIdx, styleVal);
		}
	}

	UI::ScopedStyleVar::~ScopedStyleVar() { ImGui::PopStyleVar((int)m_StyleVarListSize); }
}
