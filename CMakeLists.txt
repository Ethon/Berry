# Set up project
cmake_minimum_required(VERSION 2.8)
project(berry)

# Set appropiate flags. A C++11 compiler is required.
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

# Boost is required to build Berry.
find_package(Boost REQUIRED)

# Specify include directories.
include_directories( ${BERRY_SOURCE_DIR}/include )

# Compile berry as a shared library and link.
add_library( berry SHARED
)
target_link_libraries( berry boost_system boost_filesystem )
