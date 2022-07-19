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
		ImGui::InputText("##EntityField", buf, sizeof(buf), inputTextFlags);

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

	template<typename T>
	static void DrawFieldValue(ScriptField* field, GCHandle handle, 
		const std::function<bool(const std::string& fieldName, T& value, const ScriptType& fieldType)>& drawFunc)
	{
		T value = field->GetData<T>(handle);
		std::string fieldName = ProccessFieldName(field->GetName());
		if (drawFunc(fieldName, value, field->GetType()))
			field->SetData<T>(value, handle);
	}

	void UI::DrawScriptField(const TerranEngine::Shared<Scene>& scene, TerranEngine::ScriptField* field, const TerranEngine::GCHandle& handle)
	{
		TR_ASSERT(handle.IsValid(), "Invalid handle");
		
		ScriptClass* typeClass = field->GetType().GetTypeClass();
		std::vector<ScriptField> enumFields;
		const char** enumFieldNames = nullptr;

		if (field->GetType().IsEnum()) 
		{
			enumFields = typeClass->GetEnumFields();

			enumFieldNames = new const char* [enumFields.size()];

			for (size_t i = 0; i < enumFields.size(); i++)
				enumFieldNames[i] = enumFields[i].GetName();
		}

		switch (field->GetType().TypeEnum)
		{
		case ScriptType::Bool:
		{
			DrawFieldValue<bool>(field, handle, 
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawBoolControl(fieldName, value);
			});
			
			break;
		}
		case ScriptType::Char:
		{
			DrawFieldValue<wchar_t>(field, handle,
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				std::string strVal; strVal += value;
				bool changed = UI::DrawStringControl(fieldName, strVal, 0, 2);

				if (strVal.empty()) return false;
				value = strVal.at(0);

				return changed;
			});

			break;
		}
		case ScriptType::Int8:
		{
			DrawFieldValue<int8_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) : 
                    UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::Int16:
		{
			DrawFieldValue<int16_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) :
					UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::Int32:
		{
			DrawFieldValue<int32_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) :
					UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::Int64:
		{
			DrawFieldValue<int64_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) : 
                    UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::UInt8:
		{
			DrawFieldValue<uint8_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) : 
                    UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::UInt16:
		{
			DrawFieldValue<uint16_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) : 
                    UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::UInt32:
		{
			DrawFieldValue<uint16_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) : 
                    UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::UInt64:
		{
			DrawFieldValue<uint16_t>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return !fieldType.IsEnum() ? UI::DrawScalar(fieldName, value) : 
                    UI::DrawComboBox(fieldName, enumFieldNames, enumFields.size(), value);
			});

			break;
		}
		case ScriptType::Float:
		{
			DrawFieldValue<float>(field, handle, 
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawScalar(fieldName, value);
			});

			break;
		}
		case ScriptType::Double:
		{
			DrawFieldValue<double>(field, handle, 
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawScalar(fieldName, value);
			});

			break;
		}
		case ScriptType::String:
		{
			DrawFieldValue<std::string>(field, handle, 
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawStringControl(fieldName, value);
			});

			break;
		}
		case ScriptType::Vector2:
		{
			DrawFieldValue<glm::vec2>(field, handle, 
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawVec2Control(fieldName, value);
			});

			break;
		}
		case ScriptType::Vector3:
		{
			DrawFieldValue<glm::vec3>(field, handle, 
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawVec3Control(fieldName, value);
			});

			break;
		}
		case ScriptType::Color:
		{
			DrawFieldValue<glm::vec4>(field, handle, 
			[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawColor4Control(fieldName, value);
			});

			break;
		}
		case ScriptType::Entity:
		{
			DrawFieldValue<UUID>(field, handle, 
			[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
			{
				return UI::DrawEntityControl(fieldName, value, scene);
			});

			break;
		}
		}

		if (enumFieldNames != nullptr)
			delete[] enumFieldNames;
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

			delete[] buf;
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
