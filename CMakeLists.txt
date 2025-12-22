cmake_minimum_required(VERSION 3.28.3)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(otus_cpp_8 VERSION ${PROJECT_VESRION})
## Указываем, что требуется библиотека Boost
find_package(Boost 1.89.0 REQUIRED)
# set(Boost_INCLUDE_DIR F:/work/otus_cpp/vcpkg/installed/x64-windows/include F:/work/otus_cpp/vcpkg/installed/x64-mingw-dynamic/include)
##Подключаем директивы для использования Boost
include_directories(${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 20)

add_executable(otus_cpp_8 src/main.cpp
        src/mask_filesystem_traverser.cpp
        src/mask_filesystem_traverser.h
        src/filesystem_travercer.h
        src/filesystem_path_hash.h
        src/filesystem_path_hash.cpp
        src/duplicate_finder.h
        src/hash/crc16.cpp
        src/hash/crc16.h
        src/hash/crc32.cpp
        src/hash/crc32.h)

# Привяжите библиотеку Boost
target_link_libraries(otus_cpp_8 ${Boost_LIBRARIES})        

target_include_directories(otus_cpp_8 PRIVATE "${CMAKE_BINARY_DIR}")

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)
