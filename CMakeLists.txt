cmake_minimum_required(VERSION 3.22)

project(engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific configuration
if(MSVC)
    # Common warnings for all build types
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    
    # Debug: No optimizations, full debug info, runtime checks
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /RTC1")
    endif()

    # RelWithDebInfo: Optimizations with debug info
    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /O2 /Zi")
    endif()
    
    # Release: Maximum optimizations, no debug info
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL /LTCG")
    endif()
endif()

# File globbing
file(GLOB_RECURSE CPP_SOURCES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.h)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

# GLEW
add_subdirectory(vendor/glew/build/cmake)

# GLM
add_subdirectory(vendor/glm)

# ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
set(IMGUI_HEADERS
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/backends/imgui_impl_glfw.h
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
)

# Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/assimp)

# Executable target
add_executable(${PROJECT_NAME} 
    src/main.cpp 
    ${CPP_SOURCES} 
    ${HEADER_FILES} 
    ${IMGUI_SOURCES} 
    vendor/stb/stb_image.cpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    glew_s 
    glm 
    assimp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    vendor/glfw/include
    vendor/glew/include
    vendor/glm
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    vendor/assimp/include
    vendor/stb
    vendor/entt/src
    vendor/nlohmann_json/include
    ${CMAKE_SOURCE_DIR}/src
)