cmake_minimum_required(VERSION 3.24)
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all .cpp files in the src directory and its subdirectories
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

# Create the executable
add_executable(${PROJECT_NAME} src/main.cpp ${CPP_SOURCES} ${HEADER_FILES} ${IMGUI_SOURCES} vendor/stb/stb_image.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glew_s glm assimp)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    vendor/glfw/include
    vendor/glew/include
    vendor/glm
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    vendor/assimp/include
    vendor/stb
)
