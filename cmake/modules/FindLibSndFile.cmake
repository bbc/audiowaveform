#-------------------------------------------------------------------------------
#
# Copyright 2013 BBC Research and Development
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
# Finds libsndfile include file and library. This module sets the following
# variables:
#
#  LIBSNDFILE_FOUND       - Flag if libsndfile was found
#  LIBSNDFILE_INCLUDE_DIR - libsndfile include directory
#  LIBSNDFILE_LIBRARY     - libsndfile library path
#
#-------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

find_path(LIBSNDFILE_INCLUDE_DIR sndfile.h)
find_library(LIBSNDFILE_LIBRARY sndfile)

find_package_handle_standard_args(
    LibSndFile
    DEFAULT_MSG
    LIBSNDFILE_LIBRARY
    LIBSNDFILE_INCLUDE_DIR
)

#-------------------------------------------------------------------------------
