#pragma once

#include "ScriptTypes.h"

#include <LibCore/Assert.h>
#include <LibCore/Log.h>

#include <Coral/Type.hpp>
#include <Coral/TypeCache.hpp>

#include <LibCore/Result.h>
#include <string_view>

namespace Terran::Script {

class ManagedTypesCacheError final {
public:
    ManagedTypesCacheError(std::string_view message)
        : m_message(message)
    {
    }
    [[nodiscard]] std::string_view message() const
    {
        return m_message;
    }

private:
    std::string_view m_message;
};

class ManagedTypesCache final {
public:
    ManagedTypesCache() = default;
    ~ManagedTypesCache()
    {
        m_entity_type = nullptr;
        m_id_type = nullptr;
        m_serialize_field_type = nullptr;
        m_scriptable_type = nullptr;
    }

    static Core::Result<ManagedTypesCache, ManagedTypesCacheError> initialize_cache()
    {
        ManagedTypesCache cache;
        TR_TRACE(SCRIPT_SYSTEM, "Caching script types");
        cache.m_scriptable_type = Coral::TypeCache::Get().GetTypeByName("Terran.Scriptable");
        if (cache.m_scriptable_type == nullptr) {
            return ManagedTypesCacheError("Failed to find the Terran.Scriptable type");
        }

        cache.m_id_type = Coral::TypeCache::Get().GetTypeByName("Terran.UUID");
        if (cache.m_id_type == nullptr) {
            return ManagedTypesCacheError("Failed to find the Terran.UUID type");
        }

        // RigidbodyType = Coral::TypeCache::Get().GetTypeByName("Terran.Rigidbody2D");
        // TR_ASSERT(RigidbodyType, "Failed to find the Terran.Rigidbody2d type");
        //
        // HitInfoType = Coral::TypeCache::Get().GetTypeByName("Terran.RayCastHitInfo2D");
        // TR_ASSERT(HitInfoType, "Failed to find the Terran.RayCastHitInfo2D type");

        cache.m_entity_type = Coral::TypeCache::Get().GetTypeByName("Terran.Entity");
        if (cache.m_entity_type == nullptr) {
            return ManagedTypesCacheError("Failed to find the Terran.Entity type");
        }

        cache.m_serialize_field_type = Coral::TypeCache::Get().GetTypeByName("Terran.SerializeFieldAttribute");
        if (cache.m_serialize_field_type == nullptr) {
            return ManagedTypesCacheError("Failed to find the Terran.SerializeFieldAttribute type");
        }

        return cache;
    }

    [[nodiscard]] Coral::Type const* entity_type() const
    {
        return m_entity_type;
    }

    [[nodiscard]] Coral::Type const* scriptable_type() const
    {
        return m_scriptable_type;
    }

    [[nodiscard]] Coral::Type const* serialize_field_type() const
    {
        return m_serialize_field_type;
    }

    [[nodiscard]] Coral::Type const* id_type() const
    {
        return m_id_type;
    }

private:
    Coral::Type const* m_scriptable_type = nullptr;
    Coral::Type const* m_entity_type = nullptr;
    Coral::Type const* m_serialize_field_type = nullptr;
    Coral::Type const* m_id_type = nullptr;
    friend class Core::Result<ManagedTypesCache, ManagedTypesCacheError>;
};

}
