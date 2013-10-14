#------------------------------------------------------------------------------
#
# Copyright 2013 BBC Research and Development
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
# Notes:
#
# This file contains code adapted from OpenFLUID, (c) 2007-2010 INRA-Montpellier
# SupAgro, used under terms of the GNU General Public License version 3.
#
#------------------------------------------------------------------------------

set(OF_SYSTEM_ARCH "")
set(OF_DISTRO_ID "")
set(OF_DISTRO_CODE "")
set(OF_DISTRO_VERSION "")
set(OF_SYSTEM_IS_64BITS NO)
set(OF_SYSTEM_IS_WINDOWS NO)
set(OF_SYSTEM_IS_LINUX NO)
set(OF_SYSTEM_IS_APPLE NO)
set(OF_DISTRO_IS_UBUNTU NO)
set(OF_DISTRO_IS_FEDORA NO)
set(OF_DISTRO_IS_CENTOS NO)
set(OF_LIBDIR_SUFFIX "")

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(OF_SYSTEM_IS_64BITS YES)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OF_SYSTEM_IS_WINDOWS YES)
    if(OF_SYSTEM_IS_64BITS)
        set(OF_SYSTEM_ARCH "win64")
    else()
        set(OF_SYSTEM_ARCH "win32")
    endif()
endif()

if(UNIX AND NOT CYGWIN)
    if(NOT APPLE)
        find_program(LSB_RELEASE_PROG "lsb_release")

        if(LSB_RELEASE_PROG-NOTFOUND)
            set(OF_SYSTEM_ARCH "noarch")
            set(OF_DISTRO_ID "nodistro")
        else()
            execute_process(COMMAND "${LSB_RELEASE_PROG}" "-is" OUTPUT_VARIABLE OF_DISTRO_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
            execute_process(COMMAND "${LSB_RELEASE_PROG}" "-cs" OUTPUT_VARIABLE OF_DISTRO_CODE OUTPUT_STRIP_TRAILING_WHITESPACE)
            execute_process(COMMAND "${LSB_RELEASE_PROG}" "-rs" OUTPUT_VARIABLE OF_DISTRO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

            if(OF_DISTRO_ID STREQUAL "Ubuntu")
                set(OF_SYSTEM_IS_LINUX YES)
                set(OF_DISTRO_IS_UBUNTU YES)
                find_program(DPKG_PROG "dpkg")
                execute_process(COMMAND "${DPKG_PROG}" "--print-architecture" OUTPUT_VARIABLE OF_SYSTEM_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
            endif()

            if(OF_DISTRO_ID STREQUAL "CentOS")
                set(OF_SYSTEM_IS_LINUX YES)
                set(OF_DISTRO_IS_CENTOS YES)

                if(OF_SYSTEM_IS_64BITS)
                    set(OF_SYSTEM_ARCH "x86_64")
                    set(OF_LIBDIR_SUFFIX "64")
                else()
                    set(OF_SYSTEM_ARCH "i686")
                endif()
            endif()

            if(OF_DISTRO_ID STREQUAL "Fedora")
                set(OF_SYSTEM_IS_LINUX YES)
                set(OF_DISTRO_IS_FEDORA YES)

                if(OF_SYSTEM_IS_64BITS)
                    set(OF_SYSTEM_ARCH "x86_64")
                    set(OF_LIBDIR_SUFFIX "64")
                else()
                    set(OF_SYSTEM_ARCH "i686")
                endif()
            endif()
        endif()
    else()
        set(OF_SYSTEM_IS_APPLE YES)
    endif()
endif()
