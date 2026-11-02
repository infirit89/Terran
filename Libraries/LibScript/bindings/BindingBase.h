#pragma once

namespace Terran::Script {

#define TR_BIND_INTERNAL_FUNC(func) assembly.AddInternalCall("Terran.Internal", #func, reinterpret_cast<void*>(&func))

}
