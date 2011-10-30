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
cmake_minimum_required(VERSION 2.8)
project(berry)

# Set appropiate flags. A C++11 compiler is required.
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x")
endif()

# Boost is required to build Berry.
find_package(Boost REQUIRED)

# Specify include directories.
include_directories( include )

# Compile berry as a shared library and link.
add_library( berry SHARED
    src/process.cpp
)
target_link_libraries( berry boost_system boost_filesystem )
