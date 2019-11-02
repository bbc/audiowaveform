//------------------------------------------------------------------------------
//
// Copyright 2019 BBC Research and Development
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

#include "DurationCalculator.h"

//------------------------------------------------------------------------------

DurationCalculator::DurationCalculator() :
    sample_rate_(0),
    frame_count_(0)
{
}

//------------------------------------------------------------------------------

bool DurationCalculator::init(int sample_rate, int /* channels */, long frame_count, int /* buffer_size */)
{
    sample_rate_ = sample_rate;
    frame_count_ = frame_count;

    return true;
}

//------------------------------------------------------------------------------

bool DurationCalculator::shouldContinue() const
{
    // Only continue processing if we don't now know the length from the
    // information passed to init()
    return frame_count_ == 0;
}

//------------------------------------------------------------------------------

bool DurationCalculator::process(const short* /* input_buffer */, int input_frame_count)
{
    frame_count_ += input_frame_count;
    return true;
}

//------------------------------------------------------------------------------

void DurationCalculator::done()
{
}

//------------------------------------------------------------------------------

double DurationCalculator::getDuration() const
{
    return static_cast<double>(frame_count_) / static_cast<double>(sample_rate_);
}

//------------------------------------------------------------------------------
