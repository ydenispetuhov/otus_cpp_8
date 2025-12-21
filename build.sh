conan install . --output-folder=build --build=missing
cmake --preset conan-release
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
cmake --build .
chmod +x my_boost_pool_alloc
# chmod +x 02_logging_allocator
# chmod +x 01_std_03_allocator
# chmod +x 00_new_overload
# chmod +x 03_allocator_traits
# chmod +x 04_polymorphism
# chmod +x 05_polymorphic_allocator
# chmod +x my_boost_pool_alloc
cpack --config CPackConfig.cmake
make package_source