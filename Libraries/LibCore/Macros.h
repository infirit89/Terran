#pragma once

#define MAKE_NONCOPYABLE(CLASS_NAME)        \
    CLASS_NAME(const CLASS_NAME&) = delete; \
    CLASS_NAME& operator=(const CLASS_NAME&) = delete

#define MAKE_NONMOVEABLE(CLASS_NAME)   \
    CLASS_NAME(CLASS_NAME&&) = delete; \
    CLASS_NAME& operator=(CLASS_NAME&&) = delete
