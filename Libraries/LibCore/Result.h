#pragma once

#include "LibCore/Assert.h"
namespace Terran {
namespace Core {

// NOTE: maybe move into some ErrorMessages file?
#ifdef TR_DEBUG
#define TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE "Cannot access the value of a result when in Error variant!"
#define TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE "Cannot access the error of a result when in Ok variant!"
#else
#define TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE
#define TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE
#endif

struct Empty {};
struct EmptyError {};
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
    constexpr Result() noexcept = default;
    constexpr Result(TValue const& data) noexcept
        : m_storage(data)
        , m_isError(false)
    {
    }

    constexpr Result(TError const& error) noexcept
        : m_storage(error)
        , m_isError(true)
    {
    }
    constexpr ~Result() noexcept = default;

    constexpr bool is_ok() const {
        return !m_isError;
    }

    constexpr operator bool() const {
        return is_ok();
    }

    constexpr TValue& value() & noexcept {
        TR_ASSERT(!m_isError, TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE);
        return m_storage.data;
    }

    constexpr const TValue& value() const& noexcept {
        TR_ASSERT(!m_isError, TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE);
        return m_storage.data;
    }

    constexpr TError& error() & noexcept {
        TR_ASSERT(m_isError, TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE);
        return m_storage.error;
    }

    constexpr const TError& error() const& noexcept {
        TR_ASSERT(m_isError, TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE);
        return m_storage.error;
    }

private:
    union {
        TValue data;
        TError error;
    } m_storage = {};

    bool m_isError = false;
};

template<typename TError>
class Result<void, TError> : public Result<Empty, TError> {
public:
    using result_type = void;
    using Result<Empty, TError>::Result;
};

}

}
