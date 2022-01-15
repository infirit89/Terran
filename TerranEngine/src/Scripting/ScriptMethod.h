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

		virtual void* getPtr() = 0;

		virtual ~ScriptParameter() = default;
	};

	class Int : public ScriptParameter
	{
	public:
		Int() = default;

		Int(int value) 
			: m_Value(value)
		{}

		void* getPtr() override
		{
			return &m_Value;
		}

	private:
		int m_Value;
	};

	class ScriptMethodParameterList 
	{
	public:
		ScriptMethodParameterList() = default;
		ScriptMethodParameterList(const std::initializer_list<Shared<ScriptParameter>>& initList)
			: m_Parameters(initList)
		{}

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
		ScriptMethod(MonoMethod* monoMethod);

		// TODO: should take in an object and parameters
		void Execute(ScriptObject* scriptObject, ScriptMethodParameterList parameterList);
	private:
		MonoMethod* m_MonoMethod;
	};
} 