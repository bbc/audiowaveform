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

#include "WaveformGenerator.h"
#include "WaveformBuffer.h"
#include "Streams.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

//------------------------------------------------------------------------------

const int MAX_SAMPLE = std::numeric_limits<short>::max();
const int MIN_SAMPLE = std::numeric_limits<short>::min();

//------------------------------------------------------------------------------

WaveformGenerator::WaveformGenerator(
    WaveformBuffer& buffer,
    const int samples_per_pixel) :
    buffer_(buffer),
    channels_(0),
    samples_per_pixel_(samples_per_pixel)
{
    if (samples_per_pixel < 2) {
        throw std::runtime_error("Invalid zoom: minimum 2");
    }

    reset();
}

//------------------------------------------------------------------------------

bool WaveformGenerator::init(
    const int sample_rate,
    const int channels,
    const int /* buffer_size */)
{
    if (channels < 1 || channels > 2) {
        error_stream << "Can only generate waveform data from mono or stereo input files\n";
        return false;
    }

    channels_ = channels;

    buffer_.setSamplesPerPixel(samples_per_pixel_);
    buffer_.setSampleRate(sample_rate);

    output_stream << "Generating waveform data...\n"
                  << "Samples per pixel: " << samples_per_pixel_ << '\n'
                  << "Input channels: " << channels_ << '\n';

    return true;
}

//------------------------------------------------------------------------------

void WaveformGenerator::reset()
{
    min_ = MAX_SAMPLE;
    max_ = MIN_SAMPLE;
    count_ = 0;
}

//------------------------------------------------------------------------------

void WaveformGenerator::done()
{
    if (count_ > 0) {
        buffer_.appendSamples(static_cast<short>(min_), static_cast<short>(max_));
        reset();
    }

    output_stream << "Generated " << buffer_.getSize() << " points"
                  << std::endl;
}

//------------------------------------------------------------------------------

// See BlockFile::CalcSummary in Audacity

bool WaveformGenerator::process(
    const short* input_buffer,
    const int input_frame_count)
{
    for (int i = 0; i < input_frame_count; ++i) {
        const int index = i * channels_;

        // Sum samples from each input channel to make a single (mono) waveform
        int sample = 0;

        for (int j = 0; j < channels_; ++j) {
            sample += input_buffer[index + j];
        }

        sample /= channels_;

        // Avoid numeric overflow when converting to short
        if (sample > MAX_SAMPLE) {
            sample = MAX_SAMPLE;
        }
        else if (sample < MIN_SAMPLE) {
            sample = MIN_SAMPLE;
        }

        if (sample < min_) {
            min_ = sample;
        }

        if (sample > max_) {
            max_ = sample;
        }

        if (++count_ == samples_per_pixel_) {
            buffer_.appendSamples(static_cast<short>(min_), static_cast<short>(max_));
            reset();
        }
    }

    return true;
}

//------------------------------------------------------------------------------
