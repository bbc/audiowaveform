//------------------------------------------------------------------------------
//
// Copyright 2013-2014 BBC Research and Development
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
#include "WaveformBuffer.h"

#include "gmock/gmock.h"

//------------------------------------------------------------------------------

using testing::Eq;
using testing::Test;

//------------------------------------------------------------------------------

class WaveformRescalerTest : public Test
{
    protected:
        WaveformRescaler rescaler_;
};

//------------------------------------------------------------------------------

TEST_F(WaveformRescalerTest, shouldRescaleWaveformData)
{
    WaveformBuffer input_buffer;

    input_buffer.setSampleRate(48000);
    input_buffer.setSamplesPerPixel(512);

    input_buffer.appendSamples(0, 0);
    input_buffer.appendSamples(-10, 10);
    input_buffer.appendSamples(0, 0);
    input_buffer.appendSamples(-5, 7);
    input_buffer.appendSamples(-5, 7);
    input_buffer.appendSamples(0, 0);
    input_buffer.appendSamples(0, 0);
    input_buffer.appendSamples(0, 0);
    input_buffer.appendSamples(0, 0);
    input_buffer.appendSamples(-2, 2);

    ASSERT_THAT(input_buffer.getSize(), Eq(10));

    WaveformBuffer output_buffer;

    bool result = rescaler_.rescale(input_buffer, output_buffer, 1024);

    ASSERT_TRUE(result);
    ASSERT_THAT(output_buffer.getSize(), Eq(5));
    ASSERT_THAT(output_buffer.getSampleRate(), Eq(48000));
    ASSERT_THAT(output_buffer.getSamplesPerPixel(), Eq(1024));

    ASSERT_THAT(output_buffer.getMinSample(0), Eq(-10));
    ASSERT_THAT(output_buffer.getMaxSample(0), Eq(10));
    ASSERT_THAT(output_buffer.getMinSample(1), Eq(-5));
    ASSERT_THAT(output_buffer.getMaxSample(1), Eq(7));
    ASSERT_THAT(output_buffer.getMinSample(2), Eq(-5));
    ASSERT_THAT(output_buffer.getMaxSample(2), Eq(7));
    ASSERT_THAT(output_buffer.getMinSample(3), Eq(0));
    ASSERT_THAT(output_buffer.getMaxSample(3), Eq(0));
    ASSERT_THAT(output_buffer.getMinSample(4), Eq(-2));
    ASSERT_THAT(output_buffer.getMaxSample(4), Eq(2));
}

//------------------------------------------------------------------------------

TEST_F(WaveformRescalerTest, shouldRescale8BitWaveformData)
{
    WaveformBuffer input_buffer;
    bool result = input_buffer.load("../test/data/test_file_stereo_8bit_64spp.dat");

    ASSERT_TRUE(result);
    ASSERT_THAT(input_buffer.getSize(), Eq(1800));

    WaveformBuffer output_buffer;

    result = rescaler_.rescale(input_buffer, output_buffer, 128);

    ASSERT_TRUE(result);
    ASSERT_THAT(output_buffer.getSize(), Eq(900));
    ASSERT_THAT(output_buffer.getSampleRate(), Eq(16000));
    ASSERT_THAT(output_buffer.getSamplesPerPixel(), Eq(128));
}

//------------------------------------------------------------------------------

TEST_F(WaveformRescalerTest, shouldRescale16BitWaveformData)
{
    WaveformBuffer input_buffer;
    bool result = input_buffer.load("../test/data/test_file_stereo_16bit_64spp.dat");

    ASSERT_TRUE(result);
    ASSERT_THAT(input_buffer.getSize(), Eq(1800));

    WaveformBuffer output_buffer;

    result = rescaler_.rescale(input_buffer, output_buffer, 128);

    ASSERT_TRUE(result);
    ASSERT_THAT(output_buffer.getSize(), Eq(900));
    ASSERT_THAT(output_buffer.getSampleRate(), Eq(16000));
    ASSERT_THAT(output_buffer.getSamplesPerPixel(), Eq(128));
}

//------------------------------------------------------------------------------
