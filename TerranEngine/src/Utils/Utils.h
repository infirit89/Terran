#pragma once

#include "Core/UUID.h"
#include "Scene/Entity.h"

#include <glm/glm.hpp>

#include <string>

namespace TerranEngine 
{
	namespace Utils 
	{
		glm::vec4 NormalizeColor4(const glm::vec4& color);


		class Variant
		{
		public:
			enum class Type
			{
				None = 0,
				Bool,
				Char,
				Int8,
				Int16,
				Int32,
				Int64,
				UInt8,
				UInt16,
				UInt32,
				UInt64,
				Float,
				Double,
				String,
				Vector2,
				Vector3,
				Vector4,
				UUID
			};

			Variant();
			Variant(const Variant& other);

			Variant(int8_t i8);
			Variant(int16_t i16);
			Variant(int32_t i32);
			Variant(int64_t i64);

			Variant(uint8_t u8);
			Variant(uint16_t u16);
			Variant(uint32_t u32);
			Variant(uint64_t u64);

			Variant(bool b);

			Variant(char c);

			Variant(float f);
			Variant(double d);

			Variant(const std::string& str);
			Variant(const char* str);

			Variant(const glm::vec2& vec2);
			Variant(const glm::vec3& vec3);
			Variant(const glm::vec4& vec4);

			Variant(const UUID& id);
			Variant(Entity entity);

			Variant(char* data, const Type& type);
			
			~Variant();
			
			void Clear();

			Variant& operator=(const Variant& other)
			{
				Copy(*this, other);
				return *this;
			}
			
			operator bool() const;

			operator int8_t() const;
			operator int16_t() const;
			operator int32_t() const;
			operator int64_t() const;

			operator uint8_t() const;
			operator uint16_t() const;
			operator uint32_t() const;
			operator uint64_t() const;

			operator char() const;
			
			operator float() const;
			operator double() const;
			
			operator const char*() const;
			operator std::string() const;

			operator glm::vec2() const;
			operator glm::vec3() const;
			operator glm::vec4() const;

			operator UUID() const;

			Type GetType() const { return m_Type; }

		private:
			static void Copy(Variant& result, const Variant& in);
			
		private:
			union 
			{
				double dValue;
				int64_t iValue;
				bool bValue;
				void* ptr;
			} m_Data{};

			Type m_Type = Type::None;
		};
	}
}
