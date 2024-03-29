//------------------------------------------------------------------------------
//
// Copyright 2013-2022 BBC Research and Development
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

#include "WaveformRescaler.h"
#include "Log.h"
#include "WaveformBuffer.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>

//------------------------------------------------------------------------------

WaveformRescaler::WaveformRescaler() :
    sample_rate_(0),
    channels_(1),
    output_samples_per_pixel_(0)
{
}

//------------------------------------------------------------------------------

// See Sequence::GetWaveDisplay in Audacity

void WaveformRescaler::rescale(
    const WaveformBuffer& input_buffer,
    WaveformBuffer& output_buffer,
    int samples_per_pixel)
{
    log(Info) << "Rescaling to " << samples_per_pixel << " samples/pixel\n";

    sample_rate_ = input_buffer.getSampleRate();
    channels_ = input_buffer.getChannels();
    output_samples_per_pixel_ = samples_per_pixel;
    const int input_samples_per_pixel = input_buffer.getSamplesPerPixel();

    assert(sample_rate_ > 0);
    assert(output_samples_per_pixel_ > 0);
    assert(input_samples_per_pixel > 0);
    assert(output_samples_per_pixel_ > input_samples_per_pixel);

    const int input_buffer_size = input_buffer.getSize();

    output_buffer.setSampleRate(sample_rate_);
    output_buffer.setChannels(channels_);
    output_buffer.setSamplesPerPixel(samples_per_pixel);

    log(Info) << "Input scale: " << input_samples_per_pixel << " samples/pixel"
              << "\nOutput scale: " << samples_per_pixel << " samples/pixel"
              << "\nInput buffer size: " << input_buffer_size << '\n';

    std::vector<short> min(channels_);
    std::vector<short> max(channels_);

    if (input_buffer_size > 0) {
        for (int channel = 0; channel < channels_; ++channel) {
            min[channel] = input_buffer.getMinSample(channel, 0);
            max[channel] = input_buffer.getMaxSample(channel, 0);
        }
    }

    int input_index  = 0;
    int output_index = 0;

    int last_input_index = 0;

    while (input_index < input_buffer_size) {
        while (sampleAtPixel(output_index) / input_samples_per_pixel == input_index) {
            if (output_index > 0) {
                for (int channel = 0; channel < channels_; ++channel) {
                    output_buffer.appendSamples(min[channel], max[channel]);
                }
            }

            last_input_index = input_index;

            output_index++;

            const int where      = sampleAtPixel(output_index);
            const int prev_where = sampleAtPixel(output_index - 1);

            if (where != prev_where) {
                for (int channel = 0; channel < channels_; ++channel) {
                    min[channel] = std::numeric_limits<short>::max();
                    max[channel] = std::numeric_limits<short>::min();
                }
            }
        }

        const int where = sampleAtPixel(output_index);

        int stop = where / input_samples_per_pixel;

        if (stop > input_buffer_size) {
            stop = input_buffer_size;
        }

        while (input_index < stop) {
            for (int channel = 0; channel < channels_; ++channel) {
                short value = input_buffer.getMinSample(channel, input_index);

                if (value < min[channel]) {
                    min[channel] = value;
                }

                value = input_buffer.getMaxSample(channel, input_index);

                if (value > max[channel]) {
                    max[channel] = value;
                }
            }

            input_index++;
        }
    }

    if (input_index != last_input_index) {
        for (int channel = 0; channel < channels_; ++channel) {
            output_buffer.appendSamples(min[channel], max[channel]);
        }
    }

    log(Info) << "Generated " << output_buffer.getSize() << " points\n";
}

//------------------------------------------------------------------------------

int WaveformRescaler::sampleAtPixel(const int x) const
{
    return x * output_samples_per_pixel_;
}

//------------------------------------------------------------------------------
