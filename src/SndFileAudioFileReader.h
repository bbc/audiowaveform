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

#if !defined(INC_SND_FILE_AUDIO_FILE_READER_H)
#define INC_SND_FILE_AUDIO_FILE_READER_H

//------------------------------------------------------------------------------

#include "AudioFileReader.h"

#include <string>

#include <sndfile.h>

//------------------------------------------------------------------------------

class SndFileAudioFileReader : public AudioFileReader
{
    public:
        SndFileAudioFileReader();
        virtual ~SndFileAudioFileReader();

        SndFileAudioFileReader(const SndFileAudioFileReader&) = delete;
        SndFileAudioFileReader& operator=(const SndFileAudioFileReader&) = delete;

    public:
        virtual bool open(const char* input_filename);

        virtual bool run(AudioProcessor& processor);

    private:
        void close();

    private:
        SNDFILE* input_file_;
        SF_INFO info_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_SND_FILE_AUDIO_FILE_READER_H)

//------------------------------------------------------------------------------
