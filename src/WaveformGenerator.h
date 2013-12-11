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

#if !defined(INC_WAVEFORM_GENERATOR_H)
#define INC_WAVEFORM_GENERATOR_H

//------------------------------------------------------------------------------

#include "AudioProcessor.h"

//------------------------------------------------------------------------------

class WaveformBuffer;

//------------------------------------------------------------------------------

class WaveformGenerator : public AudioProcessor
{
    public:
        WaveformGenerator(
            WaveformBuffer& buffer,
            int samples_per_pixel
        );

        WaveformGenerator(const WaveformGenerator&) = delete;
        WaveformGenerator& operator=(const WaveformGenerator&) = delete;

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
        void reset();

    private:
        WaveformBuffer& buffer_;

        int channels_;
        int samples_per_pixel_;
        int count_;
        int min_;
        int max_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_WAVEFORM_GENERATOR_H)

//------------------------------------------------------------------------------
