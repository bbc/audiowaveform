#------------------------------------------------------------------------------
#
# Copyright 2022 BBC Research and Development
#
# Author: Chris Needham
#
# This file is part of Audio Waveform Image Generator.
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
#------------------------------------------------------------------------------

set(SYSTEM_ARCH "")
set(SYSTEM_IS_64BITS NO)
set(SYSTEM_IS_WINDOWS NO)

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(SYSTEM_IS_64BITS YES)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SYSTEM_IS_WINDOWS YES)
    if(SYSTEM_IS_64BITS)
        set(SYSTEM_ARCH "win64")
    else()
        set(SYSTEM_ARCH "win32")
    endif()
endif()

# Set SYSTEM_ARCH for use when creating Debian or RPM packages with CPack

execute_process(COMMAND "arch" OUTPUT_VARIABLE SYSTEM_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
