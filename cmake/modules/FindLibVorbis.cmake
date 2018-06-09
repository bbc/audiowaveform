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
# Finds libvorbis and libvorbisenc include files and libraries. This module sets
# the following variables:
#
#  LIBVORBIS_FOUND        - Flag if libvorbis was found
#  LIBVORBIS_INCLUDE_DIRS - libvorbis include directories
#  LIBVORBIS_LIBRARIES    - libvorbis and libvorbisenc library paths
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBVORBIS_INCLUDE_DIRS vorbis/vorbisenc.h)
find_library(LIBVORBIS_LIBRARY vorbis)
find_library(LIBVORBISENC_LIBRARY vorbisenc)

set(LIBVORBIS_LIBRARIES ${LIBVORBIS_LIBRARY} ${LIBVORBISENC_LIBRARY})

find_package_handle_standard_args(
    LibVorbis
    DEFAULT_MSG
    LIBVORBIS_LIBRARIES
    LIBVORBIS_INCLUDE_DIRS
)

#-------------------------------------------------------------------------------

