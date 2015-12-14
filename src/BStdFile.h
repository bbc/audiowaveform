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

#if !defined(INC_BSTD_FILE_H)
#define INC_BSTD_FILE_H

//------------------------------------------------------------------------------

#include <cstdio>

extern "C" {
#include "madlld-1.1p1/bstdfile.h"
}

//------------------------------------------------------------------------------

class BStdFile
{
    public:
        explicit BStdFile(FILE* file);

        BStdFile(const BStdFile&) = delete;
        BStdFile operator=(const BStdFile&) = delete;

        ~BStdFile();

    public:
        int eof() const;

        size_t read(void* buffer, size_t size, size_t count) const;

    private:
        bstdfile_t* file_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_BSTD_FILE_H)

//------------------------------------------------------------------------------
