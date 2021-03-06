# Miguel Chan - 2/28/2019
cmake_minimum_required(VERSION 3.10)
project(Enjoy-Beach)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

FILE(GLOB_RECURSE SOURCE ${SOURCE_DIR}/*.cpp ${SOURCE_DIR}/*.c ${SOURCE_DIR}/utils/*.cpp)
FILE(GLOB_RECURSE HEADER ${SOURCE_DIR}/*.h ${SOURCE_DIR}/*.hpp)

include_directories(src/includes)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fvisibility=hidden -fvisibility-inlines-hidden")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/imgui)
add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/GLM)
add_subdirectory(dependencies/GLFW)
add_subdirectory(dependencies/assimp)
add_subdirectory(dependencies/freetype)
include_directories(dependencies/GLFW/include)
include_directories(dependencies/include)
include_directories(dependencies/assimp/include)
include_directories(dependencies/freetype/include)

FILE(GLOB DEPENDENCIES_SOURCE dependencies/src/*.c dependencies/src/*.cpp)
FILE(GLOB ALL_SOURCES ${SOURCE} ${HEADER} ${DEPENDENCIES_SOURCE})

add_definitions(-DGLM_FORCE_RADIANS)

if (APPLE)
    set(os macos)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif(APPLE)

if (UNIX AND NOT APPLE)
    set(os linux)
endif(UNIX AND NOT APPLE)

if (WIN32)
    set(os windows)
    add_compile_definitions(_GLFW_WIN32)
    if (MSVC)
        add_compile_definitions(NDEBUG)
    endif(MSVC)
endif(WIN32)

set(EXECUTABLE_NAME ${PROJECT_NAME}-${os})

add_executable(${EXECUTABLE_NAME} ${ALL_SOURCES})

# copy resources and shaders after build
add_custom_target(copy-resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)


# OPENGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND) 
    message(ERROR " OPENGL not found!")
endif()

target_link_libraries(${EXECUTABLE_NAME} glfw)
target_link_libraries(${EXECUTABLE_NAME} imgui)
target_link_libraries(${EXECUTABLE_NAME} glad)
target_link_libraries(${EXECUTABLE_NAME} glm)
target_link_libraries(${EXECUTABLE_NAME} assimp)
target_link_libraries(${EXECUTABLE_NAME} freetype)
add_dependencies(${EXECUTABLE_NAME} copy-resources)
