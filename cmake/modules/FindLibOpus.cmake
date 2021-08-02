#-------------------------------------------------------------------------------
#
# Copyright 2021 BBC Research and Development
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
# Finds libopus include file and library. This module sets the following
# variables:
#
#  LIBOPUS_FOUND        - Flag if libopus was found
#  LIBOPUS_INCLUDE_DIRS - libopus include directory
#  LIBOPUS_LIBRARIES    - libopus library path
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBOPUS_INCLUDE_DIRS opus/opus.h)
find_library(LIBOPUS_LIBRARIES opus)

find_package_handle_standard_args(
    LibOpus
    DEFAULT_MSG
    LIBOPUS_LIBRARIES
    LIBOPUS_INCLUDE_DIRS
)

#-------------------------------------------------------------------------------
