cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(otus_cpp_8 VERSION ${PROJECT_VERSION})

# Ensure correct project version is used
if(NOT PROJECT_VERSION)
    message(FATAL_ERROR "PROJECT_VERSION is not set correctly.")
endif()

# point to included file and add library for Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Find Boost with required components (if any specific ones are needed, specify them)
    set(Boost_INCLUDE_DIR $ENV{VCPKG_ROOT}/installed/x64-windows/include $ENV{VCPKG_ROOT}/installed/x64-mingw-dynamic/include)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # code for Linux goes here
    find_package(Boost 1.89.0 REQUIRED)
endif()


#find_package(Boost 1.89.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files for better readability and maintenance
set(SOURCES
    src/main.cpp
    src/mask_filesystem_traverser.cpp
    src/mask_filesystem_traverser.h
    src/filesystem_travercer.h
    src/filesystem_path_hash.h
    src/filesystem_path_hash.cpp
    src/duplicate_finder.h
    src/hash/crc16.cpp
    src/hash/crc16.h
    src/hash/crc32.cpp
    src/hash/crc32.h
)

# Create executable
add_executable(otus_cpp_8 ${SOURCES})

# Link Boost libraries
target_link_libraries(otus_cpp_8 ${Boost_LIBRARIES})

# Include directories
target_include_directories(otus_cpp_8 PRIVATE "${CMAKE_BINARY_DIR}")

# Packaging configuration
set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "example@example.com")

include(CPack)