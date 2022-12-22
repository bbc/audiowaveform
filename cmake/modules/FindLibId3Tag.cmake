#-------------------------------------------------------------------------------
#
# Copyright 2016-2018 BBC Research and Development
#
# This file is part of Audio Waveform Image Generator.
#
# Author: Chris Needham
#
# Audio Waveform Image Generator is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# Audio Waveform Image Generator is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# Audio Waveform Image Generator.  If not, see <http://www.gnu.org/licenses/>.
#
#-------------------------------------------------------------------------------
#
# Finds libid3 include file and library. This module sets the following
# variables:
#
#  LIBID3TAG_FOUND        - Flag if libid3tag was found
#  LIBID3TAG_INCLUDE_DIRS - libid3tag include directories
#  LIBID3TAG_LIBRARIES    - libid3tag library paths
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBID3TAG_INCLUDE_DIRS id3tag.h)
find_library(LIBID3TAG_LIBRARIES id3tag)

if(BUILD_STATIC)
    if (LIBID3TAG_LIBRARIES STREQUAL LIBID3TAG_LIBRARIES-NOTFOUND)
        message(FATAL_ERROR "libid3tag library (${CMAKE_FIND_LIBRARY_SUFFIXES}) not found")
    endif()

    find_package(ZLIB REQUIRED)

    list(
        APPEND
        LIBID3TAG_INCLUDE_DIRS
        ${ZLIB_INCLUDE_DIR}
    )

    list(
        APPEND
        LIBID3TAG_LIBRARIES
        ${ZLIB_LIBRARY}
    )
endif()

find_package_handle_standard_args(
    LibId3Tag
    DEFAULT_MSG
    LIBID3TAG_LIBRARIES
    LIBID3TAG_INCLUDE_DIRS
)

#-------------------------------------------------------------------------------
