#include "trpch.h"
#include "ManagedMetadata.h"

#include "ScriptEngine.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/image.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/tokentype.h>

namespace TerranEngine 
{
    std::vector<ManagedClass> ManagedMetadata::GetClassListFromMetadata(Shared<ScriptAssembly> assembly)
    {
        std::vector<ManagedClass> classList;
        const MonoTableInfo* typedefTableInfo = mono_image_get_table_info(assembly->GetMonoImage(), MONO_TABLE_TYPEDEF);
        const int typedefTableRows = mono_table_info_get_rows(typedefTableInfo);
        for (int i = 1; i < typedefTableRows; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typedefTableInfo, i, cols, MONO_TYPEDEF_SIZE);
            uint32_t typeToken = ((i + 1) | MONO_TOKEN_TYPE_DEF);
            classList.push_back(assembly->GetClassFromTypeToken(typeToken));
        }
        return classList;
    }
    uint32_t ManagedMetadata::GetMethodCountForClass(Shared<ScriptAssembly> assembly, const ManagedClass* klass)
    {
        const MonoTableInfo* typedefTableInfo = mono_image_get_table_info(assembly->GetMonoImage(), MONO_TABLE_TYPEDEF);
        const MonoTableInfo* methodTableInfo = mono_image_get_table_info(assembly->GetMonoImage(), MONO_TABLE_METHOD);
        const int methodTableRows = mono_table_info_get_rows(methodTableInfo);

        if (methodTableRows <= 0)
            return 0;

        const int typedefTableRows = mono_table_info_get_rows(typedefTableInfo);
        const int klassTypedefIndex = (klass->GetTypeToken() & ~MONO_TOKEN_TYPE_DEF) - 1;

        uint32_t nextCol[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typedefTableInfo, std::min(klassTypedefIndex + 1, typedefTableRows - 1),
            nextCol, MONO_TYPEDEF_SIZE);

        return std::min(nextCol[MONO_TYPEDEF_METHOD_LIST], (uint32_t)methodTableRows);
    }
}
