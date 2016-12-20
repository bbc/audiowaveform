#-------------------------------------------------------------------------------
#
# Copyright 2013, 2014 BBC Research and Development
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

#-------------------------------------------------------------------------------
#
# CMake project setup
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.7)
project(audiowaveform)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: default is Release")
endif()

message(STATUS "CMAKE_VERSION=${CMAKE_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_MODULE_PATH "${audiowaveform_SOURCE_DIR}/cmake/modules")
message(STATUS "CMAKE_MODULE_PATH='${CMAKE_MODULE_PATH}'")

include(SystemInfo)

#-------------------------------------------------------------------------------
#
# Version number
#
#-------------------------------------------------------------------------------

# Read version number from VERSION file and split into its component parts.

file(STRINGS "VERSION" VERSION)

string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" VERSION_PARTS ${VERSION})

set(VERSION_MAJOR ${CMAKE_MATCH_1})
set(VERSION_MINOR ${CMAKE_MATCH_2})
set(VERSION_PATCH ${CMAKE_MATCH_3})

message(STATUS "Building version ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

#-------------------------------------------------------------------------------
#
# Dependencies
#
#-------------------------------------------------------------------------------

find_package(LibGD REQUIRED)
if(LIBGD_FOUND)
    message(STATUS "LIBGD_INCLUDE_DIR='${LIBGD_INCLUDE_DIR}'")
    message(STATUS "LIBGD_LIBRARY=${LIBGD_LIBRARY}")
    include_directories(${LIBGD_INCLUDE_DIR})
endif(LIBGD_FOUND)

find_package(LibSndFile REQUIRED)
if(LIBSNDFILE_FOUND)
    message(STATUS "LIBSNDFILE_INCLUDE_DIR='${LIBSNDFILE_INCLUDE_DIR}'")
    message(STATUS "LIBSNDFILE_LIBRARY=${LIBSNDFILE_LIBRARY}")
    include_directories(${LIBSNDFILE_INCLUDE_DIR})
endif(LIBSNDFILE_FOUND)

find_package(LibMad REQUIRED)
if(LIBMAD_FOUND)
    message(STATUS "LIBMAD_INCLUDE_DIR='${LIBMAD_INCLUDE_DIR}'")
    message(STATUS "LIBMAD_LIBRARY=${LIBMAD_LIBRARY}")
    include_directories(${LIBMAD_INCLUDE_DIR})
endif(LIBMAD_FOUND)

find_package(LibId3Tag REQUIRED)
if(LIBID3TAG_FOUND)
    message(STATUS "LIBID3TAG_INCLUDE_DIR='${LIBID3TAG_INCLUDE_DIR}'")
    message(STATUS "LIBID3TAG_LIBRARY=${LIBID3TAG_LIBRARY}")
    include_directories(${LIBID3TAG_INCLUDE_DIR})
endif(LIBID3TAG_FOUND)

find_package(Boost 1.46.0 COMPONENTS program_options filesystem regex system REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS='${Boost_INCLUDE_DIRS}'")
    message(STATUS "Boost_LIBRARIES='${Boost_LIBRARIES}'")
    include_directories(${Boost_INCLUDE_DIRS})
endif(Boost_FOUND)

#-------------------------------------------------------------------------------
#
# Packaging
#
#-------------------------------------------------------------------------------

set(CPACK_PACKAGE_NAME "audiowaveform")
set(CPACK_PACKAGE_VENDOR "Chris Needham")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT "Chris Needham <chris@chrisneedham.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Audio waveform data and image generator")
set(CPACK_PACKAGE_DESCRIPTION "Generates audio waveform data that can be used to render waveform images, similar to Audacity")

set(CPACK_RESOURCE_FILE_LICENSE "${audiowaveform_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${audiowaveform_SOURCE_DIR}/README.md")
set(PACKAGE_RELEASE_NUMBER 1)

if(UNIX AND NOT APPLE AND NOT CYGWIN)
    if(OF_DISTRO_IS_UBUNTU)
        set(CPACK_GENERATOR "DEB")
        set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${OF_SYSTEM_ARCH}")

        set(PACKAGE_BOOST_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libmad0 (>=0.15.1), libsndfile1 (>= 1.0.25), libgd2-xpm (>= 2.0.35), libboost-program-options${PACKAGE_BOOST_VERSION}, libboost-filesystem${PACKAGE_BOOST_VERSION}, libboost-regex${PACKAGE_BOOST_VERSION}")

        # http://www.debian.org/doc/manuals/debian-faq/ch-pkg_basics.en.html#s-pkgname
        # The Debian binary package file names conform to the following convention:
        # <foo>_<VersionNumber>-<DebianRevisionNumber>_<DebianArchitecture>.deb
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}-${PACKAGE_RELEASE_NUMBER}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    endif()

    if(OF_DISTRO_IS_CENTOS OR OF_DISTRO_IS_FEDORA)
        set(CPACK_GENERATOR "RPM")
        set(CPACK_RPM_PACKAGE_GROUP "Applications/Multimedia")
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "${OF_SYSTEM_ARCH}")
        set(CPACK_RPM_PACKAGE_REQUIRES "libmad >= 0.15.1, libsndfile >= 1.0.25, gd >= 2.0.35, boost >= ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}")

        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${PACKAGE_RELEASE_NUMBER}.${OF_SYSTEM_ARCH}")
    endif()
endif()

include(CPack)

#-------------------------------------------------------------------------------
#
# Compiler flags
#
#-------------------------------------------------------------------------------

if(CMAKE_VERSION VERSION_LESS "2.8.10")
    exec_program(
        ${CMAKE_CXX_COMPILER}
        ARGS --version
        OUTPUT_VARIABLE COMPILER_VERSION_STRING
    )
    string(REGEX REPLACE ".*([0-9]\\.[0-9]\\.[0-9]).*" "\\1" CMAKE_CXX_COMPILER_VERSION ${COMPILER_VERSION_STRING})
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.6.3")
    message(FATAL_ERROR "g++ 4.6.3 or later required")
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.7")
    set(CMAKE_CXX_FLAGS "-std=c++11")
else()
    # Set GTEST_LANG_CXX11=0 to disable C++11 features when compiling googlemock.
    # Without this compilation fails with g++ 4.6.3 on gmock-matchers.h.
    set(CMAKE_CXX_FLAGS "-std=c++0x -DGTEST_LANG_CXX11=0")
endif()

set(COMMON_FLAGS "-Wall -Wextra -Wconversion -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -DBOOST_FILESYSTEM_NO_DEPRECATED")
set(CMAKE_C_FLAGS ${COMMON_FLAGS})

if(APPLE)
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()

message(STATUS "CMAKE_CXX_COMPILER_VERSION='${CMAKE_CXX_COMPILER_VERSION}'")
message(STATUS "CMAKE_CXX_FLAGS='${CMAKE_CXX_FLAGS}'")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG='${CMAKE_CXX_FLAGS_DEBUG}'")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE='${CMAKE_CXX_FLAGS_RELEASE}'")
message(STATUS "CMAKE_CXX_COMPILE_OBJECT='${CMAKE_CXX_COMPILE_OBJECT}'")

#-------------------------------------------------------------------------------
#
# Source files
#
#-------------------------------------------------------------------------------

include_directories(src)

# Configure a header file to pass some of the CMake settings to the source code.
configure_file(
    "${PROJECT_SOURCE_DIR}/src/Config.h.in"
    "${PROJECT_BINARY_DIR}/Config.h"
)

# Add the binary directory to the search path for include files so that we find
# Config.h.
include_directories("${PROJECT_BINARY_DIR}")

set(MODULES
    src/AudioFileReader.cpp
    src/AudioProcessor.cpp
    src/BStdFile.cpp
    src/Error.cpp
    src/GdImageRenderer.cpp
    src/MathUtil.cpp
    src/Mp3AudioFileReader.cpp
    src/Options.cpp
    src/OptionHandler.cpp
    src/Rgba.cpp
    src/SndFileAudioFileReader.cpp
    src/TimeUtil.cpp
    src/WaveformBuffer.cpp
    src/WaveformColors.cpp
    src/WaveformGenerator.cpp
    src/WaveformRescaler.cpp
    src/WavFileWriter.cpp
    src/madlld-1.1p1/bstdfile.c
)

set(SRCS
    src/Main.cpp
    ${MODULES}
)

add_executable(audiowaveform ${SRCS})

#-------------------------------------------------------------------------------
#
# Linker
#
#-------------------------------------------------------------------------------

# Specify libraries to link against.
set(LIBS ${LIBSNDFILE_LIBRARY} ${LIBGD_LIBRARY} ${LIBMAD_LIBRARY} ${LIBID3TAG_LIBRARY} ${Boost_LIBRARIES})
target_link_libraries(audiowaveform ${LIBS})

#-------------------------------------------------------------------------------
#
# Tests
#
#-------------------------------------------------------------------------------

if (NOT DEFINED ENABLE_TESTS)
    set(ENABLE_TESTS 1)
endif()

if(ENABLE_TESTS)
    enable_testing()

    add_subdirectory(googlemock)

    set(TESTS
        test/AudioFileReaderTest.cpp
        test/GdImageRendererTest.cpp
        test/MathUtilTest.cpp
        test/Mp3AudioFileReaderTest.cpp
        test/OptionsTest.cpp
        test/OptionHandlerTest.cpp
        test/RgbaTest.cpp
        test/SndFileAudioFileReaderTest.cpp
        test/TimeUtilTest.cpp
        test/WavFileWriterTest.cpp
        test/WaveformBufferTest.cpp
        test/WaveformGeneratorTest.cpp
        test/WaveformRescalerTest.cpp
        test/util/FileDeleter.cpp
        test/util/FileUtil.cpp
        test/util/Streams.cpp
    )

    include_directories(${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include)
    set(TEST_LIBS gmock_main)
    add_executable(audiowaveform_tests ${MODULES} ${TESTS})
    target_link_libraries(audiowaveform_tests ${LIBS} ${TEST_LIBS})
    add_test(audiowaveform_tests audiowaveform_tests)
else()
    message(STATUS "Unit tests disabled")
endif()

#-------------------------------------------------------------------------------
#
# Documentation
#
#-------------------------------------------------------------------------------

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc)

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/doc/audiowaveform.1.gz
    COMMAND gzip -c -9 ${PROJECT_SOURCE_DIR}/doc/audiowaveform.1 > ${PROJECT_BINARY_DIR}/doc/audiowaveform.1.gz
    DEPENDS ${PROJECT_SOURCE_DIR}/doc/audiowaveform.1
)

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/doc/audiowaveform.5.gz
    COMMAND gzip -c -9 ${PROJECT_SOURCE_DIR}/doc/audiowaveform.5 > ${PROJECT_BINARY_DIR}/doc/audiowaveform.5.gz
    DEPENDS ${PROJECT_SOURCE_DIR}/doc/audiowaveform.5
)

add_custom_target(doc
    DEPENDS ${PROJECT_BINARY_DIR}/doc/audiowaveform.1.gz
            ${PROJECT_BINARY_DIR}/doc/audiowaveform.5.gz
)

add_dependencies(audiowaveform doc)

#-------------------------------------------------------------------------------
#
# Installation
#
#-------------------------------------------------------------------------------

message(STATUS "CMAKE_INSTALL_PREFIX='${CMAKE_INSTALL_PREFIX}'")

# Install executable
install(TARGETS audiowaveform DESTINATION bin)

# Install man pages
install(
    FILES ${PROJECT_BINARY_DIR}/doc/audiowaveform.1.gz
    DESTINATION share/man/man1
)

install(
    FILES ${PROJECT_BINARY_DIR}/doc/audiowaveform.5.gz
    DESTINATION share/man/man5
)

#-------------------------------------------------------------------------------
