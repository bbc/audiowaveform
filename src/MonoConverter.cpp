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

#include "MonoConverter.h"
#include "Streams.h"

#include <sndfile.h>

#include <cstring>
#include <iostream>

//------------------------------------------------------------------------------

MonoConverter::MonoConverter(const char* output_filename) :
    output_filename_(output_filename),
    output_file_(nullptr),
    channels_(0),
    buffer_size_(0)
{
}

//------------------------------------------------------------------------------

MonoConverter::~MonoConverter()
{
    close();
}

//------------------------------------------------------------------------------

bool MonoConverter::init(
    const int sample_rate,
    const int channels,
    const int buffer_size)
{
    if (channels != 2) {
        error_stream << "Input file must be stereo to use MonoConverter\n";
        return false;
    }

    channels_    = channels;
    buffer_size_ = buffer_size;

    output_buffer_.resize(static_cast<std::vector<short>::size_type>(buffer_size_));

    SF_INFO info;
    std::memset(&info, 0, sizeof(info));

    info.samplerate = sample_rate;
    info.channels   = 1;
    info.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    output_file_ = sf_open(output_filename_.c_str(), SFM_WRITE, &info);

    if (output_file_ != nullptr) {
        output_stream << "Writing output file: " << output_filename_ << std::endl;
    }
    else {
        error_stream << sf_strerror(output_file_) << '\n';
    }

    return output_file_ != nullptr;
}

//------------------------------------------------------------------------------

void MonoConverter::close()
{
    if (output_file_ != nullptr) {
        sf_close(output_file_);
        output_file_ = nullptr;
    }
}

//------------------------------------------------------------------------------

bool MonoConverter::process(
    const short* input_buffer,
    const int input_frame_count)
{
    for (int i = 0; i < input_frame_count; ++i) {
        int frame_index = i * channels_;

        output_buffer_[i] = static_cast<short>(
            (input_buffer[frame_index] + input_buffer[frame_index + 1]) / 2
        );
    }

    sf_count_t frames_written = sf_writef_short(
        output_file_,
        &output_buffer_[0],
        input_frame_count
    );

    return frames_written == input_frame_count;
}

//------------------------------------------------------------------------------

void MonoConverter::done()
{
    close();
}

//------------------------------------------------------------------------------
