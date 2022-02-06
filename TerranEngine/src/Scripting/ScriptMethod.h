#pragma once

#include "Core/Base.h"

#include <mono/metadata/debug-helpers.h>

namespace TerranEngine 
{
	class ScriptObject;

	class ScriptParameter 
	{
	public:
		ScriptParameter() = default;
		virtual ~ScriptParameter() = default;

		virtual void* getPtr() = 0;
	};

	class Bool : public ScriptParameter
	{
	public:
		Bool() = default;
		Bool(bool value)
			: m_Value(value) {}
		Bool(const Bool& other) = default;

		~Bool() = default;

		void* getPtr() override { return &m_Value; }

	private:
		bool m_Value;
	};

	class Char : public ScriptParameter
	{
	public:
		Char() = default;
		Char(char value)
			: m_Value(value) {}
		Char(const Char& other) = default;

		~Char() = default;

		void* getPtr() override { return &m_Value; }

	private:
		char m_Value;
	};

	class Int : public ScriptParameter
	{
	public:
		Int() = default;
		Int(int value) 
			: m_Value(value) {}
		Int(const Int& other) = default;

		~Int() = default;

		void* getPtr() override { return &m_Value; }

	private:
		int m_Value;
	};

	class UInt32 : public ScriptParameter
	{
	public:
		UInt32() = default;
		UInt32(uint32_t value)
			: m_Value(value) {}
		UInt32(const UInt32& other) = default;

		~UInt32() = default;

		void* getPtr() override { return &m_Value; }

	private:
		uint32_t m_Value;
	};

	class Float : public ScriptParameter
	{
	public:
		Float() = default;
		Float(float value)
			: m_Value(value) {}
		Float(const Float& other) = default;
		
		~Float() = default;

		void* getPtr() override { return &m_Value; }

	private:
		float m_Value;
	};

	class Double : public ScriptParameter
	{
	public:
		Double() = default;
		Double(double value)
			: m_Value(value) {}
		Double(const Double& other) = default;

		~Double() = default;

		void* getPtr() override { return &m_Value; }

	private:
		double m_Value;
	};

	class ScriptMethodParameterList 
	{
	public:
		ScriptMethodParameterList() = default;
		ScriptMethodParameterList(const std::initializer_list<Shared<ScriptParameter>>& initList)
			: m_Parameters(initList) {}
		ScriptMethodParameterList(const ScriptMethodParameterList& other) = default;

		~ScriptMethodParameterList() = default;

		void** GetRawParams() 
		{
			void** raw = new void*[m_Parameters.size()];

			for (size_t i = 0; i < m_Parameters.size(); i++)
				raw[i] = m_Parameters[i]->getPtr();

			return raw;
		}
	private:
		std::vector<Shared<ScriptParameter>> m_Parameters;
	};

	class ScriptMethod 
	{
	public:
		ScriptMethod() = default;
		ScriptMethod(MonoMethod* monoMethod);
		ScriptMethod(const ScriptMethod& other) = default;

		~ScriptMethod() = default;

		void Invoke(Shared<ScriptObject> scriptObject, ScriptMethodParameterList parameterList = {});
		
	private:
		MonoMethod* m_MonoMethod = nullptr;
	};
} 