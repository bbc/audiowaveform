//------------------------------------------------------------------------------
//
// Copyright 2023 BBC Research and Development
//
// Author: Chris Needham
//
// This file is part of Audio Waveform Image Generator.
//
// Audio Waveform Image Generator is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// Audio Waveform Image Generator is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Audio Waveform Image Generator.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------

#include "FileUtil.h"
#include "Streams.h"

#include <cstring>
#include <fstream>
#include <iostream>

#include <sys/stat.h>

//------------------------------------------------------------------------------

namespace FileUtil {

//------------------------------------------------------------------------------

bool isStdioFilename(const char* filename)
{
    return filename == nullptr ||
           filename[0] == '\0' ||
           (strcmp(filename, "-") == 0);
}

//------------------------------------------------------------------------------

#ifdef _WIN32
#define S_ISSOCK(mode) 0
#endif

//------------------------------------------------------------------------------

bool isStdinSeekable()
{
    struct stat stat_buf;

    int result = fstat(fileno(stdin), &stat_buf);

    if (result >= 0) {
        if (S_ISFIFO(stat_buf.st_mode) || S_ISSOCK(stat_buf.st_mode)) {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

const char* getInputFilename(const char* filename)
{
    return FileUtil::isStdioFilename(filename) ? "(stdin)" : filename;
}

//------------------------------------------------------------------------------

const char* getOutputFilename(const char* filename)
{
    return FileUtil::isStdioFilename(filename) ? "(stdout)" : filename;
}

//------------------------------------------------------------------------------

} // namespace FileUtil

//------------------------------------------------------------------------------
