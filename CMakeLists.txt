cmake_minimum_required(VERSION 3.24)
project(engine)

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

# Create the executable
add_executable(${PROJECT_NAME} src/main.cpp ${CPP_SOURCES} ${HEADER_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glew_s glm)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE vendor/glfw/include vendor/glew/include vendor/glm)
