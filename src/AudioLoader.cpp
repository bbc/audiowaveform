//------------------------------------------------------------------------------
//
// Copyright 2022 BBC Research and Development
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

#include "AudioLoader.h"

#include "Log.h"

#include <ostream>

//------------------------------------------------------------------------------

AudioLoader::AudioLoader() :
    sample_rate_(0),
    channels_(0)
{
}

//------------------------------------------------------------------------------

bool AudioLoader::init(int sample_rate, int channels, long /* frame_count */, int /* buffer_size */)
{
    sample_rate_ = sample_rate;
    channels_ = channels;

    return true;
}

//------------------------------------------------------------------------------

bool AudioLoader::shouldContinue() const
{
    return true;
}

//------------------------------------------------------------------------------

bool AudioLoader::process(const short* input_buffer, int input_frame_count)
{
    for (int i = 0; i < input_frame_count * channels_; i++) {
        audio_samples_.push_back(input_buffer[i]);
    }

    return true;
}

//------------------------------------------------------------------------------

void AudioLoader::done()
{
}

//------------------------------------------------------------------------------

double AudioLoader::getDuration() const
{
    const size_t frame_count = audio_samples_.size() / channels_;

    return static_cast<double>(frame_count) / static_cast<double>(sample_rate_);
}

//------------------------------------------------------------------------------
