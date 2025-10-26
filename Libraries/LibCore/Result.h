#pragma once

#include "LibCore/Assert.h"
#include <LibCore/Macros.h>
#include <type_traits>
#include <utility>
namespace Terran {
namespace Core {

// NOTE: maybe move into some ErrorMessages file?
#ifdef TR_DEBUG
#    define TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE "Cannot access the value of a result when in Error variant!"
#    define TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE "Cannot access the error of a result when in Ok variant!"
#else
#    define TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE
#    define TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE
#endif

struct Empty { };
struct EmptyError { };
// NOTE: new thing learned, if the base definition of result
// is temlate<typename T, typename Err = Empty> Result,
// when result is Result<void>
// due to the partial specialization it becomes Result<Empty, Empty>
// but then both constructors that take in TValue and TError, take in Empty and Empty
// thus an error occurs
template<typename TValue, typename TError = EmptyError>
class Result {
public:
    using result_type = TValue;
    using error_type = TError;

    MAKE_NONCOPYABLE(Result);

    constexpr Result()
    requires(std::is_default_constructible_v<TValue>)
    = default;

    template<typename U>
    constexpr Result(U const& data) noexcept
    requires(std::is_convertible_v<U, TValue>)
        : m_storage(data)
        , m_isError(false)
    {
    }

    template<typename E>
    constexpr Result(E const& error) noexcept
    requires(std::is_convertible_v<E, TError>)
        : m_isError(true)
    {
        m_storage.error = error;
    }

    constexpr ~Result() noexcept = default;

    [[nodiscard]] constexpr bool is_ok() const
    {
        return !m_isError;
    }

    constexpr operator bool() const
    {
        return is_ok();
    }

    [[nodiscard]] constexpr TValue& value() & noexcept
    {
        TR_ASSERT(!m_isError, TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE);
        return m_storage.data;
    }

    [[nodiscard]] constexpr TValue const& value() const& noexcept
    {
        TR_ASSERT(!m_isError, TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE);
        return m_storage.data;
    }

    [[nodiscard]] constexpr TError& error() & noexcept
    {
        TR_ASSERT(m_isError, TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE);
        return m_storage.error;
    }

    [[nodiscard]] constexpr TError const& error() const& noexcept
    {
        TR_ASSERT(m_isError, TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE);
        return m_storage.error;
    }

private:
    union storage {
        TValue data;
        TError error;
        ~storage() { }
    } m_storage = {};

    bool m_isError;
};

template<typename TError>
class Result<void, TError> : public Result<Empty, TError> {
public:
    using result_type = void;
    using Result<Empty, TError>::Result;
};

template<typename TError = EmptyError>
class Bad {
public:
    Bad()
    requires(std::is_default_constructible_v<TError>)
    = default;
    Bad(TError const& error)
        : m_error(error)
    {
    }

    operator Result<void, TError>() const { return { m_error }; }

    template<typename TValue>
    operator Result<TValue, TError>() const { return { m_error }; }

private:
    TError m_error;
};

}

}
