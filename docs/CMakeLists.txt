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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
# ***************************************************************************

if(CASC_MASTER_PROJECT)
    #####################################################################
    # Target to generate Doxygen documentation
    #####################################################################
    find_package(Doxygen)

    if(DOXYGEN_FOUND)
        option(CASC_DOCS_ADVANCED "Expose private functions and details in docs?" OFF)
        set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
        set(DOXYGEN_PROJECT_NUMBER ${VERSION})
        set(DOXYGEN_CLANG_ASSISTED_PARSING "YES")
        set(DOXYGEN_CLANG_OPTIONS "-stdlib=libc++ -std=c++14 -w")

        if(CASC_DOCS_ADVANCED)
            set(DOXYGEN_EXTRACT_PRIVATE "YES")
            set(DOXYGEN_ENABLED_SECTIONS "detail")
        endif()


        doxygen_add_docs(docs
            ${CMAKE_SOURCE_DIR}/README.md
            ${CMAKE_SOURCE_DIR}/include/casc
        )
    endif()


endif(CASC_MASTER_PROJECT)
