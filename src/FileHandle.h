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

#if !defined(INC_FILE_HANDLE_H)
#define INC_FILE_HANDLE_H

//------------------------------------------------------------------------------

#include <cstdio>

//------------------------------------------------------------------------------

class FileHandle
{
    public:
        FileHandle();
        FileHandle(const FileHandle& buffer) = delete;
        FileHandle& operator=(const FileHandle& buffer) = delete;

        ~FileHandle();

    public:
        bool open(const char* filename);
        void close();
        FILE* get() const;
        int getFileDescriptor();
        bool isOpen() const;
        bool isStdio() const;
        bool hasError() const;
        long getFilePos() const;

    private:
        FILE* file_;
        bool close_;
};

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
