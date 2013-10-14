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

#if !defined(INC_WAV_FILE_WRITER_H)
#define INC_WAV_FILE_WRITER_H

//------------------------------------------------------------------------------

#include "AudioProcessor.h"

#include <sndfile.h>

#include <string>
#include <vector>

//------------------------------------------------------------------------------

class WavFileWriter : public AudioProcessor
{
    public:
        WavFileWriter(const char* output_filename);
        ~WavFileWriter();

        WavFileWriter(const WavFileWriter&) = delete;
        WavFileWriter& operator=(const WavFileWriter&) = delete;

    public:
        virtual bool init(
            int sample_rate,
            int channels,
            int buffer_size
        );

        virtual bool process(
            const short* input_buffer,
            int input_frame_count
        );

        virtual void done();

    private:
        void close();

    private:
        std::string output_filename_;
        SNDFILE* output_file_;
        int channels_;
        std::vector<short> output_buffer_;
        int buffer_size_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_WAV_FILE_WRITER_H)

//------------------------------------------------------------------------------
