//------------------------------------------------------------------------------
//
// Copyright 2023 Chris Needham
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

#include "FileHandle.h"
#include "FileUtil.h"
#include "Log.h"

#include <cerrno>
#include <cstring>
#include <iostream>

//------------------------------------------------------------------------------

FileHandle::FileHandle()
    :   file_(nullptr),
        close_(false)
{
}

//------------------------------------------------------------------------------

FileHandle::~FileHandle()
{
    close();
}

//------------------------------------------------------------------------------

void FileHandle::close()
{
    if (file_ != nullptr) {
        if (close_) {
            fclose(file_);
        }

        file_ = nullptr;
    }
}

//------------------------------------------------------------------------------

bool FileHandle::open(const char* filename)
{
    if (FileUtil::isStdioFilename(filename)) {
        file_ = stdin;
        close_ = false;
    }
    else {
        file_ = fopen(filename, "rb");

        if (file_ == nullptr) {
            log(Error) << "Failed to read file: " << filename << '\n'
                       << strerror(errno) << '\n';
            return false;
        }

        close_ = true;
    }

    return true;
}

//------------------------------------------------------------------------------

FILE* FileHandle::get() const
{
    return file_;
}

//------------------------------------------------------------------------------

int FileHandle::getFileDescriptor()
{
    return fileno(file_);
}

//------------------------------------------------------------------------------

bool FileHandle::isOpen() const
{
    return file_ != nullptr;
}

//------------------------------------------------------------------------------

bool FileHandle::isStdio() const
{
    return !close_;
}

//------------------------------------------------------------------------------

bool FileHandle::hasError() const
{
    return ferror(file_);
}

//------------------------------------------------------------------------------

long FileHandle::getFilePos() const
{
    return ftell(file_);
}

//------------------------------------------------------------------------------
