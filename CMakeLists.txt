cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(Template VERSION 1.0.0)

# Add all source files inside src (recursively)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

include(FetchContent)
FetchContent_Declare(
    GeodeSDK
    GIT_REPOSITORY https://github.com/geode-sdk/geode.git

    GIT_TAG        main
)
FetchContent_MakeAvailable(GeodeSDK)
add_subdirectory(${geodesdk_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/geode)


# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})
