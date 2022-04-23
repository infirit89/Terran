#pragma once

#include <memory>

#define TR_EVENT_BIND_FN(event) std::bind(&event, this, std::placeholders::_1)

namespace TerranEngine
{
	template<typename T>
	using Unique = std::unique_ptr<T>;

	template<typename T, typename... Args>
	Unique<T> CreateUnique(Args&&... args) 
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Shared = std::shared_ptr<T>;

	template<typename T, typename... Args>
	Shared<T> CreateShared(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Weak = std::weak_ptr<T>;

	template<typename T, typename... Args>
	Weak<T> CreateWeak(Args&&... args)
	{
		return std::make_weak<T>(std::forward<Args>(args)...);
	}

	template<typename T1, typename T2>
	Shared<T1> DynamicCast(const Shared<T2>& ptr) 
	{
		return std::dynamic_pointer_cast<T1, T2>(ptr);
	}
}
