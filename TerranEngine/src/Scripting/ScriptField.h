#pragma once

typedef struct _MonoClassField MonoClassField;
typedef struct _MonoObject MonoObject;

namespace TerranEngine 
{
	enum class ScriptFieldType
	{
		None = 0,
		Bool,
		Char,
		Int,
		Int8,
		Int16,
		Int64,
		UInt8,
		UInt16,
		UInt,
		UInt64,
		Float,
		Double,
		String,
		Vector2,
		Vector3
	};

	enum class ScriptFieldVisiblity 
	{
		Unknown,
		Private,
		Public,
		Protected,
		Internal
	};

	class ScriptField 
	{
	public:
		ScriptField() = default;
		ScriptField(MonoClassField* monoField, MonoObject* monoObject);
		
		ScriptField(const ScriptField& other) = default;
		~ScriptField() = default;

		inline const char* GetName() const					{ return m_Name; }
		inline ScriptFieldType GetType() const				{ return m_FieldType; }
		inline ScriptFieldVisiblity GetVisibility() const	{ return m_FieldVisibility; }

		void SetDataRaw(void* value);
		void GetDataRaw(void* result);

		const char* GetDataStringRaw();
		void SetDataStringRaw(const char* value);

		template<typename T>
		T GetData() 
		{
			T value;
			GetDataRaw(&value);
			return value;
		}

		template<typename T>
		void SetData(T value) 
		{
			SetDataRaw(&value);
		}

		template<>
		const char* GetData<const char*>() 
		{
			const char* value = GetDataStringRaw();
			return value;
		}

		template<>
		void SetData<const char*>(const char* value) 
		{
			SetDataStringRaw(value);
		}

		// NOTE: maybe marshal vec2 and vec3?

	private:
		MonoClassField* m_MonoField = nullptr;
		const char* m_Name = nullptr;
		MonoObject* m_MonoObject = nullptr;
		ScriptFieldType m_FieldType = ScriptFieldType::None;
		ScriptFieldVisiblity m_FieldVisibility = ScriptFieldVisiblity::Unknown;
	};
}
