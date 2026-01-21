/**
 * @file Result.h
 * @brief Result type.
 * @ingroup LibCore
 */
#pragma once

#include "Assert.h"
#include <type_traits>
namespace Terran::Core {

// NOTE: maybe move into some ErrorMessages file?
#ifdef TR_DEBUG
#    define TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE "Cannot access the value of a result when in Error variant!"
#    define TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE "Cannot access the error of a result when in Ok variant!"
#else
#    define TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE
#    define TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE
#endif

/**
 * @struct Empty
 * @brief Empty result, used for Result<void>
 */
struct Empty { };

/**
 * @struct EmptyError
 * @brief Empty error, used when you want to signify that there is some error
 */
struct EmptyError { };
// NOTE: new thing learned, if the base definition of result
// is temlate<typename T, typename Err = Empty> Result,
// when result is Result<void>
// due to the partial specialization it becomes Result<Empty, Empty>
// but then both constructors that take in TValue and TError, take in Empty and Empty
// thus an error occurs

/**
 * @class Result
 * @brief Result type used for indicating success with data or errors
 */
template<typename TValue, typename TError = EmptyError>
class Result {
public:
    using result_type = TValue;
    using error_type = TError;

    constexpr Result() noexcept
    requires(std::is_default_constructible_v<TValue>)
    = default;

    /**
     * @brief Create a success type
     * @tparam U The success type to be created
     * @param data The success value
     */
    template<typename U>
    constexpr Result(U const& data) noexcept
    requires(std::is_convertible_v<U, TValue>)
        : m_storage(data)
        , m_isError(false)
    {
    }

    /**
     * @brief Create an error type
     * @tparam E The error type to be created
     * @param error The error value
     */
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

    /**
     * @brief Tries to get value, if the type is error, it asserts
     */
    [[nodiscard]] constexpr TValue& value() & noexcept
    {
        TR_ASSERT(!m_isError, TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE);
        return m_storage.data;
    }

    /**
     * @brief Tries to get value, if the type is error, it asserts
     */
    [[nodiscard]] constexpr TValue const& value() const& noexcept
    {
        TR_ASSERT(!m_isError, TR_RESULT_VALUE_ACCESS_WHEN_ERROR_STATE_MESSAGE);
        return m_storage.data;
    }

    /**
     * @brief Tries to get error, if the type is success, it asserts
     */
    [[nodiscard]] constexpr TError& error() & noexcept
    {
        TR_ASSERT(m_isError, TR_RESULT_ERROR_ACCESS_WHEN_VALUE_STATE_MESSAGE);
        return m_storage.error;
    }

    /**
     * @brief Tries to get error, if the type is success, it asserts
     */
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

/**
 * @class Result<void>
 * @brief Specialization for when the result success type is void
 */
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
