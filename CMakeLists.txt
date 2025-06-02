cmake_minimum_required(VERSION 3.10)
project(image_tests)

# Find libpng
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add the executable
file(GLOB IMAGE_TEST_SOURCES "image_tests/*.cpp")
add_executable(image_tests ${IMAGE_TEST_SOURCES} ../src/image.cpp)

# Add include directories
target_include_directories(image_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${JPEG_INCLUDE_DIR})  # Ensure JPEG include directories are added

target_link_libraries(image_tests PRIVATE
  PNG::PNG
  ${JPEG_LIBRARY}      # explicitly link to libjpeg
  gtest_main
)

# Enable 12-bit JPEG support
target_compile_definitions(image_tests PRIVATE WITH_12BIT)

# Add test
add_test(NAME image_tests COMMAND image_tests)

# Optional: ensure linker can find libjpeg at runtime
link_directories(/usr/local/lib)
