#pragma once

#include "AssetTypes.h"

#include <filesystem>

namespace Terran::Asset {

class AssetRemovedEvent {
public:
    constexpr AssetRemovedEvent(AssetId const& handle) noexcept
        : m_handle(handle)
    {
    }

    [[nodiscard]] constexpr AssetId const& handle() const noexcept
    {
        return m_handle;
    }

private:
    AssetId m_handle;
};

class AssetRenamedEvent {
public:
    AssetRenamedEvent(AssetId const& handle, std::filesystem::path const& new_file_name, std::filesystem::path const& old_file_name)
        : m_handle(handle)
        , m_new_file_name(new_file_name)
        , m_old_file_name(old_file_name)
    {
    }

    [[nodiscard]] constexpr AssetId const& handle() const noexcept
    {
        return m_handle;
    }

    [[nodiscard]] constexpr std::filesystem::path const& old_file_name() const noexcept
    {
        return m_old_file_name;
    }

    [[nodiscard]] constexpr std::filesystem::path const& new_file_name() const noexcept
    {
        return m_new_file_name;
    }

private:
    AssetId m_handle;
    std::filesystem::path m_new_file_name;
    std::filesystem::path m_old_file_name;
};

}
