//------------------------------------------------------------------------------
//
// Copyright 2015 BBC Research and Development
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

#include "BStdFile.h"
#include "Error.h"

#include <cstring>

//------------------------------------------------------------------------------

BStdFile::BStdFile(FILE* file)
{
    file_ = NewBstdFile(file);

    if (file_ == nullptr) {
        throwError("Can't create a new bstdfile_t: %1%", strerror(errno));
    }
}

//------------------------------------------------------------------------------

BStdFile::~BStdFile()
{
    if (file_ != nullptr) {
        BstdFileDestroy(file_);
        file_ = nullptr;
    }
}

//------------------------------------------------------------------------------

int BStdFile::eof() const
{
    return BstdFileEofP(file_);
}

//------------------------------------------------------------------------------

size_t BStdFile::read(void* buffer, size_t size, size_t count) const
{
    return BstdRead(buffer, size, count, file_);
}

//------------------------------------------------------------------------------
