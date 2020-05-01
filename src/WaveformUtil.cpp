//------------------------------------------------------------------------------
//
// Copyright 2020 BBC Research and Development
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

#include "WaveformUtil.h"
#include "MathUtil.h"
#include "WaveformBuffer.h"

#include <cassert>
#include <cmath>
#include <limits>

//------------------------------------------------------------------------------

namespace WaveformUtil {

//------------------------------------------------------------------------------

// Returns the minimum and maximum values over a given region of the buffer.

static std::pair<int, int> getAmplitudeRange(
    const WaveformBuffer& buffer,
    int start_index,
    int end_index)
{
    int low  = std::numeric_limits<int>::max();
    int high = std::numeric_limits<int>::min();

    const int channels = buffer.getChannels();

    for (int i = start_index; i != end_index; ++i) {
        for (int channel = 0; channel < channels; ++channel) {
            const int min = buffer.getMinSample(channel, i);
            const int max = buffer.getMaxSample(channel, i);

            if (min < low) {
                low = min;
            }

            if (max > high) {
                high = max;
            }
        }
    }

    return std::make_pair(low, high);
}

//------------------------------------------------------------------------------

double getAmplitudeScale(
    const WaveformBuffer& buffer,
    int start_index,
    int end_index)
{
    assert(start_index >= 0);
    assert(start_index <= buffer.getSize());

    assert(end_index >= 0);
    assert(end_index <= buffer.getSize());

    assert(end_index > start_index);

    const std::pair<int, int> range = getAmplitudeRange(buffer, start_index, end_index);

    const double amplitude_scale_high = (range.second == 0) ? 1.0 : 32767.0 / range.second;
    const double amplitude_scale_low  = (range.first  == 0) ? 1.0 : 32767.0 / range.first;

    return std::fabs(std::min(amplitude_scale_high, amplitude_scale_low));
}

//------------------------------------------------------------------------------

void scaleWaveformAmplitude(WaveformBuffer& buffer, double amplitude_scale)
{
    const int size = buffer.getSize();
    const int channels = buffer.getChannels();

    for (int i = 0; i != size; ++i) {
        for (int channel = 0; channel < channels; ++channel) {
            const short min = MathUtil::scale(
                buffer.getMinSample(channel, i),
                amplitude_scale
            );

            const short max = MathUtil::scale(
                buffer.getMaxSample(channel, i),
                amplitude_scale
            );

            buffer.setSamples(channel, i, min, max);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace WaveformUtil

//------------------------------------------------------------------------------
