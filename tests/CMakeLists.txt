# ***************************************************************************
# This file is part of the Colored Abstract Simplicial Complex library.
# Copyright (C) 2016-2017
# by Christopher Lee, John Moody, Rommie Amaro, J. Andrew McCammon,
#    and Michael Holst
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# ***************************************************************************

include(FetchContent)
FetchContent_Declare(
    googletest
    # URL https://github.com/google/googletest/archive/master.zip
    URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz
    SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-src"
    BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/googletest-build"
)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${googletest_SOURCE_DIR}
                     ${googletest_BINARY_DIR}
                     EXCLUDE_FROM_ALL)
endif()

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${googletest_SOURCE_DIR}/include")
endif()

add_executable(casctests
                    main.cpp
                    CASCTests.cpp
                    SimplexSetTests.cpp
                    FunctionsTests.cpp
                    DecimationTests.cpp
                    TraversalTests.cpp
                    IndexTrackerTests.cpp
                    )
target_link_libraries(casctests gtest_main casc)
# target_compile_options(casctests PRIVATE -Werror)
# target_compile_options(casctests PRIVATE -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion -ferror-limit=3)
# target_compile_options(casctests PRIVATE -Wno-c++17-extensions)
# target_compile_options(casctests PRIVATE -Wno-unused-variable -Wno-unused-parameter)

add_test(NAME CascTests COMMAND casctests)

add_executable(cascscratch cascscratch.cpp)
target_link_libraries(cascscratch casc)

