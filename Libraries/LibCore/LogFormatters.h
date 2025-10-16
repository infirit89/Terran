#pragma once

#include <glm/detail/qualifier.hpp>
#include <glm/detail/setup.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/format.h>

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>> {
    static constexpr auto parse(format_parse_context const& ctx) -> decltype(ctx.begin())
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(glm::vec<1, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}", vec[0]);
    }

    template<typename FormatContext>
    auto format(glm::vec<2, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}", vec[0], vec[1]);
    }

    template<typename FormatContext>
    auto format(glm::vec<3, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}, {}", vec[0], vec[1], vec[2]);
    }

    template<typename FormatContext>
    auto format(glm::vec<4, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}, {}, {}", vec[0], vec[1], vec[2], vec[3]);
    }
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> {
    static constexpr auto parse(format_parse_context const& ctx) -> decltype(ctx.begin())
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(glm::qua<T, Q> const& qua, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}, {}, {}", qua.x, qua.y, qua.z, qua.w);
    }
};
