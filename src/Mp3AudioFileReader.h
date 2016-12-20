//------------------------------------------------------------------------------
//
// Copyright 2013 BBC Research and Development
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

#if !defined(INC_MP3_AUDIO_FILE_READER_H)
#define INC_MP3_AUDIO_FILE_READER_H

//------------------------------------------------------------------------------

#include "AudioFileReader.h"

#include <cstdio>

//------------------------------------------------------------------------------

class Mp3AudioFileReader : public AudioFileReader
{
    public:
        Mp3AudioFileReader();
        virtual ~Mp3AudioFileReader();

        Mp3AudioFileReader(const Mp3AudioFileReader&) = delete;
        Mp3AudioFileReader& operator=(Mp3AudioFileReader&) = delete;

    public:
        virtual bool open(const char* input_filename);

        virtual bool run(AudioProcessor& processor);

    private:
        void close();
        bool getFileSize();
        bool skipId3Tags();

    private:
        FILE* file_;
        long file_size_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_MP3_AUDIO_FILE_READER_H)

//------------------------------------------------------------------------------
