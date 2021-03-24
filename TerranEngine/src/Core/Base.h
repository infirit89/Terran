#pragma once

#include <memory>

#define TR_EVENT_BIND_FN(event) std::bind(&event, this, std::placeholders::_1)