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
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -DNDEBUG")
endif()

# Boost is required to build Berry.
find_package(Boost REQUIRED)

# Specify include directories.
include_directories( include )

# Detect operating system.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(OPERATING_SYSTEM "linux")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(WIN32)
    set(OPERATING_SYSTEM "win32")
endif(WIN32)

# Compile berry as a shared library.
file(GLOB BERRY_SOURCE_FILES "src/*.cpp")
file(GLOB BERRY_SYSTEM_SOURCE_FILES "src/${OPERATING_SYSTEM}/*.cpp")
add_library( berry SHARED
    ${BERRY_SOURCE_FILES}
    ${BERRY_SYSTEM_SOURCE_FILES}
)

# Link libraries.
target_link_libraries( berry
  boost_system
  boost_filesystem
)

# Compile Python bindings if wanted.
option(COMPILE_PYTHON_BINDINGS "Turn on to compile Python bindings" ON)
if(COMPILE_PYTHON_BINDINGS)
    add_subdirectory(python)
endif(COMPILE_PYTHON_BINDINGS)

# Compile tests if wanted.
option(COMPILE_TESTS "Turn on to compile tests" OFF)
if(COMPILE_TESTS)
endif(COMPILE_TESTS)

# Install.
file(GLOB_RECURSE BERRY_HEADER_FILES include/berry/*.hpp)
file(GLOB_RECURSE BERRY_DETAIL_HEADER_FILES include/berry/detail/*.hpp)
install(TARGETS berry DESTINATION lib)
install(FILES ${BERRY_HEADER_FILES} DESTINATION include/berry)
install(FILES ${BERRY_DETAIL_HEADER_FILES} DESTINATION include/berry/detail)