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
# Finds libflac include file and library. This module sets the following
# variables:
#
#  LIBFLAC_FOUND        - Flag if libflac was found
#  LIBFLAC_INCLUDE_DIRS - libflac include directories
#  LIBFLAC_LIBRARIES    - libflac library paths
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBFLAC_INCLUDE_DIRS FLAC/all.h)
find_library(LIBFLAC_LIBRARIES FLAC)

find_package_handle_standard_args(
    LibFLAC
    DEFAULT_MSG
    LIBFLAC_LIBRARIES
    LIBFLAC_INCLUDE_DIRS
)

#-------------------------------------------------------------------------------

