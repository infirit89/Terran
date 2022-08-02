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

	void UI::SetupImGuiStyle()
	{
		// Gruvbox style from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0;
		//style.DisabledAlpha = 0.6000000238418579;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 0.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(3.900000095367432f, 3.0f);
		style.FrameRounding = 0.0f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.TabBorderSize = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.9742489457130432f, 0.9573628306388855f, 0.8864410519599915f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.658823549747467f, 0.6000000238418579f, 0.5176470875740051f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9215686321258545f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.5882353186607361f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.5882353186607361f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.7843137383460999f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.9803921580314636f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1176470592617989f, 0.1098039224743843f, 0.105882354080677f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03529411926865578f, 0.0470588244497776f, 0.05490196123719215f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1568627506494522f, 0.1411764770746231f, 0.1333333402872086f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.4000000059604645f, 0.3607843220233917f, 0.3294117748737335f, 0.5882353186607361f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 0.5882353186607361f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 0.7843137383460999f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 0.8823529481887817f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7450980544090271f, 0.1176470592617989f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.7450980544090271f, 0.1176470592617989f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8235294222831726f, 0.196078434586525f, 0.1568627506494522f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.4901960790157318f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.686274528503418f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.8823529481887817f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.3921568691730499f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.5882353186607361f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.7843137383460999f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 0.6000000238418579f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6980392336845398f, 1.0f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.8980392217636108f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.7764706015586853f, 0.8196078538894653f, 1.0f, 0.6000000238418579f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.7764706015586853f, 0.8196078538894653f, 1.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.4313725531101227f, 0.105882354080677f, 0.062745101749897f, 0.8823529481887817f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5490196347236633f, 0.105882354080677f, 0.062745101749897f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.5882353186607361f, 0.105882354080677f, 0.062745101749897f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.4313725531101227f, 0.105882354080677f, 0.062745101749897f, 0.8823529481887817f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.5882353186607361f, 0.105882354080677f, 0.062745101749897f, 0.7843137383460999f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.9843137264251709f, 0.9450980424880981f, 0.7803921699523926f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.2705882489681244f, 0.5215686559677124f, 0.5333333611488342f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.843137264251709f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.2705882489681244f, 0.5215686559677124f, 0.5333333611488342f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.407843142747879f, 0.3568627536296844f, 0.3137255012989044f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.4941176474094391f, 0.43529412150383f, 0.3764705955982208f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1568627506494522f, 0.1411764770746231f, 0.1333333402872086f, 0.501960813999176f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.1568627506494522f, 0.1411764770746231f, 0.1333333402872086f, 0.501960813999176f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.02745098061859608f, 0.4000000059604645f, 0.4705882370471954f, 1.0f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4470588266849518f, 0.4470588266849518f, 0.8980392217636108f, 0.800000011920929f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7019608020782471f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3499999940395355f);
	}
}
