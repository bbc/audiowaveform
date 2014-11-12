//------------------------------------------------------------------------------
//
// Copyright 2013, 2014 BBC Research and Development
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

#include <boost/format.hpp>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

//------------------------------------------------------------------------------

ScaleFactor::~ScaleFactor()
{
}

//------------------------------------------------------------------------------

SamplesPerPixelScaleFactor::SamplesPerPixelScaleFactor(int samples_per_pixel) :
    samples_per_pixel_(samples_per_pixel)
{
}

//------------------------------------------------------------------------------

int SamplesPerPixelScaleFactor::getSamplesPerPixel(int /* sample_rate */) const
{
    return samples_per_pixel_;
}

//------------------------------------------------------------------------------

// Calculates samples_per_pixel such that the time range start_time to end_time
// fits the specified image width.

DurationScaleFactor::DurationScaleFactor(
    double start_time,
    double end_time,
    int width_pixels) :
    start_time_(start_time),
    end_time_(end_time),
    width_pixels_(width_pixels)
{
    if (end_time < start_time) {
        const std::string message = boost::str(
            boost::format("Invalid end time, must be greater than %1%") % start_time
        );

        throw std::runtime_error(message);
    }

    if (width_pixels < 1) {
        throw std::runtime_error("Invalid image width: minimum 1");
    }
}

//------------------------------------------------------------------------------

int DurationScaleFactor::getSamplesPerPixel(const int sample_rate) const
{
    const double seconds = end_time_ - start_time_;

    const int width_samples = static_cast<int>(seconds * sample_rate);

    const int samples_per_pixel = width_samples / width_pixels_;

    return samples_per_pixel;
}

//------------------------------------------------------------------------------

PixelsPerSecondScaleFactor::PixelsPerSecondScaleFactor(int pixels_per_second) :
    pixels_per_second_(pixels_per_second)
{
    if (pixels_per_second_ <= 0) {
        throw std::runtime_error("Invalid pixels per second: must be greater than zero");
    }
}

//------------------------------------------------------------------------------

int PixelsPerSecondScaleFactor::getSamplesPerPixel(int sample_rate) const
{
    return sample_rate / pixels_per_second_;
}

//------------------------------------------------------------------------------

const int MAX_SAMPLE = std::numeric_limits<short>::max();
const int MIN_SAMPLE = std::numeric_limits<short>::min();

//------------------------------------------------------------------------------

WaveformGenerator::WaveformGenerator(
    WaveformBuffer& buffer,
    const ScaleFactor& scale_factor) :
    buffer_(buffer),
    scale_factor_(scale_factor),
    channels_(0),
    samples_per_pixel_(0)
{
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

    samples_per_pixel_ = scale_factor_.getSamplesPerPixel(sample_rate);

    if (samples_per_pixel_ < 2) {
        error_stream << "Invalid zoom: minimum 2\n";
        return false;
    }

    buffer_.setSamplesPerPixel(samples_per_pixel_);
    buffer_.setSampleRate(sample_rate);

    output_stream << "Generating waveform data...\n"
                  << "Samples per pixel: " << samples_per_pixel_ << '\n'
                  << "Input channels: " << channels_ << '\n';

    return true;
}

//------------------------------------------------------------------------------

int WaveformGenerator::getSamplesPerPixel() const
{
    return samples_per_pixel_;
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
