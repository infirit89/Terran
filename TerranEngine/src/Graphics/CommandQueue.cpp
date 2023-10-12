#include "trpch.h"
#include "CommandQueue.h"

namespace TerranEngine 
{
	CommandQueue::CommandQueue(uint32_t capacity)
		: m_CommandCapacity(capacity)
	{
		m_CommandQueueBase = new uint8_t[capacity];
		m_CommandQueuePtr = m_CommandQueueBase;
		m_CommandCount = 0;
	}

	CommandQueue::~CommandQueue()
	{
		delete[] m_CommandQueueBase;
	}

	void CommandQueue::Execute()
	{
		// should use the base casue then we loose the original memory location
		uint8_t* temp = m_CommandQueueBase;
		for (size_t i = 0; i < m_CommandCount; i++)
		{
			CommandFn fn = *(CommandFn*)temp;
			uint32_t size = *(uint32_t*)(temp += sizeof(CommandFn));
			temp += sizeof(uint32_t);
			fn(temp);
			temp += size;
		}

		m_CommandCount = 0;
		m_CommandQueuePtr = m_CommandQueueBase;
	}

	void* CommandQueue::Allocate(CommandFn func, uint32_t size)
	{
		uint32_t dataSize = (uint32_t)((m_CommandQueuePtr + sizeof(func) + sizeof(uint32_t) + size) - m_CommandQueueBase);

		// NOTE: resize ?
		TR_ASSERT(dataSize <= m_CommandCapacity, "Command queue overflow");

		*(CommandFn*)m_CommandQueuePtr = func;
		m_CommandQueuePtr += sizeof(CommandFn);

		*(uint32_t*)m_CommandQueuePtr = size;
		m_CommandQueuePtr += sizeof(uint32_t);

		void* funcArgs = m_CommandQueuePtr;
		m_CommandQueuePtr += size;

		m_CommandCount++;
		return funcArgs;
	}
}
