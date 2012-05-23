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

# Boost and Python required to build Berry.
find_package(Boost 1.42.0 COMPONENTS system python filesystem REQUIRED)
find_package(PythonLibs REQUIRED)

# Add required header files to include path.
include_directories(
   ${PYTHON_INCLUDE_DIRS}
)

# Compile pyberry as shared library.
file(GLOB PYBERRY_SOURCE_FILES "*.cpp")
add_library(pyberry SHARED
   ${PYBERRY_SOURCE_FILES}
)

# Link to required libraries.
target_link_libraries(pyberry
   ${Boost_LIBRARIES}
   ${PYTHON_LIBRARIES}
   ${BERRY_LIBRARIES}
)
