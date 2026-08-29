cmake_minimum_required(VERSION 4.4)

project(simulation C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-O3")

# Add Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE) # enable wayland on linux
set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE) # disable x11 on linux (switch these settings around as you please)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

# Add source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/")

# Declaring executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

