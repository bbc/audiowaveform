#-------------------------------------------------------------------------------
#
# Copyright 2018 BBC Research and Development
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
# Finds libogg include file and library. This module sets the following
# variables:
#
#  LIBOGG_FOUND        - Flag if libogg was found
#  LIBOGG_INCLUDE_DIRS - libogg include directory
#  LIBOGG_LIBRARIES    - libogg library path
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBOGG_INCLUDE_DIRS ogg/ogg.h)
find_library(LIBOGG_LIBRARIES ogg)

find_package_handle_standard_args(
    LibOGG
    DEFAULT_MSG
    LIBOGG_LIBRARIES
    LIBOGG_INCLUDE_DIRS
)

#-------------------------------------------------------------------------------

