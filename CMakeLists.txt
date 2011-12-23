# This file is part of Berry.
# 
# Berry is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Berry is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Berry. If not, see <http://www.gnu.org/licenses/>.

# Set up project
cmake_minimum_required(VERSION 2.6)
project(berry)

# Set appropiate flags. A C++11 compiler is required.
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -Weffc++ -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s")
endif()

# Define options.
option(COMPILE_TESTS "Turn on to compile tests" OFF)

# Boost is required to build Berry.
find_package(Boost REQUIRED)

# Specify include directories.
include_directories( include )

# Compile berry as a shared library.
file(GLOB BERRY_SOURCE_FILES src/*.cpp)
add_library( berry SHARED
  ${BERRY_SOURCE_FILES}
)

# Link libraries.
target_link_libraries( berry
  boost_system
  boost_filesystem
)

# Compile tests if wanted.
if(COMPILE_TESTS)
  add_executable(test_processes
    test/test_processes.cpp
  )

  target_link_libraries( test_processes
    berry
    boost_system
    boost_filesystem
    boost_unit_test_framework
  )
endif(COMPILE_TESTS)

# Install.
file(GLOB_RECURSE BERRY_HEADER_FILES include/*.hpp)
install(TARGETS berry DESTINATION lib)
install(FILES ${BERRY_HEADER_FILES} DESTINATION include)
