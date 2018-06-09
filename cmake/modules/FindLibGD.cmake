#-------------------------------------------------------------------------------
#
# Copyright 2013-2018 BBC Research and Development
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
# Finds libgd include file and library. This module sets the following
# variables:
#
#  LIBGD_FOUND        - Flag if libgd was found
#  LIBGD_INCLUDE_DIRS - libgd include directories
#  LIBGD_LIBRARIES    - libgd library paths
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBGD_INCLUDE_DIRS gd.h)
find_library(LIBGD_LIBRARIES gd)

if (BUILD_STATIC)
    find_package(PNG REQUIRED)

    list(
        APPEND
        LIBGD_LIBRARIES
        ${PNG_LIBRARIES}
    )

    list(
        APPEND
        LIBGD_INCLUDE_DIRS
        ${PNG_INCLUDE_DIRS}
    )
endif()

find_package_handle_standard_args(
    LibGD
    DEFAULT_MSG
    LIBGD_LIBRARIES
    LIBGD_INCLUDE_DIRS
)

#-------------------------------------------------------------------------------
