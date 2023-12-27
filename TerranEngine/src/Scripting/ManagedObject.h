#pragma once

extern "C"
{
	typedef struct _MonoObject MonoObject;
}

namespace TerranEngine 
{
	class ManagedClass;

	class ManagedObject 
	{
	public:
		ManagedObject() = default;
		ManagedObject(MonoObject* monoObject);

		ManagedObject(const ManagedObject& other) = default;
		~ManagedObject() = default;

		inline MonoObject* GetMonoObject() const { return m_MonoObject; }
		inline operator MonoObject* () const { return m_MonoObject; }
		
		//static ManagedObject CreateInstace(const ManagedClass& klass);

		ManagedClass* GetClass() { return m_Class; }

		inline operator bool() const { return m_MonoObject; }  
		
	private:
		MonoObject* m_MonoObject;
		ManagedClass* m_Class;

		friend class ManagedClass;
	};
}
