#pragma once

#include "Core/Base.h"

namespace TerranEngine 
{
	class CommandQueue 
	{
	public:
		CommandQueue(uint32_t capacity = 10_mb);
		~CommandQueue();

		template<typename FuncT>
		void Submit(FuncT&& fn)
		{
			auto func = [](void* ptr)
			{
				auto f = (FuncT*)ptr;
				(*f)();
			};

			void* funcArgs = Allocate(func, sizeof(fn));
			new (funcArgs) FuncT(std::forward<FuncT>(fn));
		}

		// exucte and clear
		void Execute();

	private:
		typedef void(*CommandFn)(void*);
		void* Allocate(CommandFn func, uint32_t size);
		uint8_t* m_CommandQueueBase, * m_CommandQueuePtr;
		uint32_t m_CommandCount, m_CommandCapacity;
	};
}
