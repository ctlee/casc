# ***************************************************************************
# This file is part of the Colored Abstract Simplicial Complex library.
# Copyright (C) 2016-2021
# by Christopher T. Lee, John Moody, Rommie Amaro, J. Andrew McCammon,
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

cmake_minimum_required(VERSION 3.10)

# Disable in source builds
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
# Add path to custom modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake-modules")

#####################################################################
# Project GAMer
#####################################################################
project(CASC VERSION 1.0.5 

DESCRIPTION "Simplicial complex data structure for arbitrary dimensions with stored data"
HOMEPAGE_URL "https://ctlee.github.io/casc/"
LANGUAGES CXX)

# Require c++14 and standard libraries
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Check if casc is being used directly or via add_subdirectory
set(CASC_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CASC_MASTER_PROJECT ON)
endif()

#####################################################################
# Options
#####################################################################
option(CASC_INSTALL "Install casc header files?" ${CASC_MASTER_PROJECT})
option(BUILD_CASCTESTS "Build the test scripts?" ${CASC_MASTER_PROJECT})
# option(BUILD_CASCEXAMPLES "Build the CASC surface mesh example?" ${CASC_MASTER_PROJECT})
option(EXTERNAL_GTEST "Look for an external GoogleTest?" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CASC_VERSION ${CASC_VERSION_MAJOR}.${CASC_VERSION_MINOR}.${CASC_VERSION_PATCH})
message(STATUS "casc v${CASC_VERSION}")

# Cache variables
set(CASC_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include" CACHE INTERNAL "")

include(GNUInstallDirs)

add_library(casc INTERFACE)
target_include_directories(casc INTERFACE
        $<BUILD_INTERFACE:${CASC_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

if(CASC_INSTALL)
    install(DIRECTORY ${CASC_INCLUDE_DIR}/casc DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    # TODO (0): Also setup cmake support files cascConfig.cmake etc.
endif()

if(BUILD_CASCTESTS)
    enable_testing()
    add_subdirectory(tests)
endif(BUILD_CASCTESTS)

# if(BUILD_CASCEXAMPLES)
#     add_subdirectory(examples)
# endif(BUILD_CASCEXAMPLES)

add_subdirectory(docs)
