#pragma once

#include "Core/Base.h"

#include "ScriptObject.h"

#include <mono/metadata/debug-helpers.h>

namespace TerranEngine 
{
	class Parameter 
	{
	public:
		Parameter() = default;

		virtual void* getPtr() = 0;

		virtual ~Parameter() = default;
	};

	class Int : public Parameter
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
		ScriptMethodParameterList(std::initializer_list<Shared<Parameter>> initList) 
			: m_Parameters(initList)
		{}

		void Insert(Shared<Int> intParam) 
		{
			m_Parameters.push_back(intParam);
		}

		void** GetRawParams() 
		{
			void** raw = new void*[m_Parameters.size()];
			
			for (size_t i = 0; i < m_Parameters.size(); i++)
				raw[i] = m_Parameters[i]->getPtr();

			return raw;
		}

	private:
		std::vector<Shared<Parameter>> m_Parameters;
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