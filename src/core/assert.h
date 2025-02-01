#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace Engine {

#define UNUSED(x) (void)(x)

// Disable in release builds
#ifdef NDEBUG
    #define ASSERT(condition, ...) ((void)0)
#else
    // Platform-specific debug break
    #if defined(_MSC_VER)
        #define DEBUG_BREAK() __debugbreak()
    #else
        #define DEBUG_BREAK() raise(SIGTRAP)
    #endif

    // Main ASSERT macro
    #define ASSERT(condition, ...) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "Assertion Failed: %s\nFile: %s\nLine: %d\n", \
                    #condition, __FILE__, __LINE__); \
                fprintf(stderr, "Message: " __VA_ARGS__); \
                DEBUG_BREAK(); \
                abort(); \
            } \
        } while(0)
#endif

}