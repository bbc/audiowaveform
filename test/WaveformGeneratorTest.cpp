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

#include "WaveformBuffer.h"
#include "WaveformGenerator.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

#include <climits>
#include <stdexcept>

//------------------------------------------------------------------------------

using testing::EndsWith;
using testing::Eq;
using testing::HasSubstr;
using testing::Test;

//------------------------------------------------------------------------------

class WaveformGeneratorTest : public Test
{
    protected:
        virtual void SetUp()
        {
            output.str(output_str);
            error.str(error_str);
        }

        virtual void TearDown()
        {
        }

        std::string output_str;
        std::string error_str;
};

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldThrowIfSamplesPerPixelIsInvalid)
{
    WaveformBuffer buffer;

    ASSERT_THROW(WaveformGenerator generator(buffer, INT_MIN), std::runtime_error);
    ASSERT_THROW(WaveformGenerator generator(buffer, 0), std::runtime_error);
    ASSERT_THROW(WaveformGenerator generator(buffer, 1), std::runtime_error);
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldConstructIfSamplesPerPixelIsValid)
{
    WaveformBuffer buffer;

    ASSERT_NO_THROW(WaveformGenerator generator(buffer, 2));
    ASSERT_NO_THROW(WaveformGenerator generator(buffer, INT_MAX));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldNotProcessMonoAudio)
{
    WaveformBuffer buffer;

    const int samples_per_pixel = 300;

    WaveformGenerator generator(buffer, samples_per_pixel);

    const int sample_rate = 44100;
    const int channels    = 1;
    const int BUFFER_SIZE = 1024;

    bool success = generator.init(sample_rate, channels, BUFFER_SIZE);
    ASSERT_FALSE(success);

    std::string str = error.str();
    ASSERT_THAT(str, HasSubstr("stereo"));
    ASSERT_THAT(str, EndsWith("\n"));
    ASSERT_TRUE(output.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldSetBufferAttributes)
{
    WaveformBuffer buffer;

    const int samples_per_pixel = 300;

    WaveformGenerator generator(buffer, samples_per_pixel);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);

    ASSERT_THAT(buffer.getSampleRate(), Eq(44100));
    ASSERT_THAT(buffer.getSamplesPerPixel(), Eq(300));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldComputeMaxAndMinValues)
{
    WaveformBuffer buffer;

    const int samples_per_pixel = 300;

    WaveformGenerator generator(buffer, samples_per_pixel);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    short samples[BUFFER_SIZE];
    memset(samples, 0, sizeof(samples));

    const int frames = BUFFER_SIZE / channels;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);

    // even indexes: left channel, odd indexes: right channel
    samples[0] = 100;
    samples[1] = 102;
    samples[200] = 98;
    samples[201] = 100;
    samples[400] = -98;
    samples[401] = -100;
    samples[598] = -100;
    samples[599] = -102;

    samples[600] = 197;
    samples[601] = 199;
    samples[800] = -200;
    samples[801] = -202;
    samples[900] = -197;
    samples[901] = -199;
    samples[1022] = 200;
    samples[1023] = 202;

    result = generator.process(samples, frames);
    ASSERT_TRUE(result);

    generator.done();

    // Check contents of buffer
    ASSERT_THAT(buffer.getSampleRate(), Eq(44100));
    ASSERT_THAT(buffer.getSamplesPerPixel(), Eq(300));
    ASSERT_THAT(buffer.getSize(), Eq(2)); // 512 / 300 = 1 remainder 212
                                          // => 2 output points total

    // Check valuess are average of left and right channels
    ASSERT_THAT(buffer.getMinSample(0), Eq(-101));
    ASSERT_THAT(buffer.getMaxSample(0), Eq(101));

    ASSERT_THAT(buffer.getMinSample(1), Eq(-201));
    ASSERT_THAT(buffer.getMaxSample(1), Eq(201));
}

//------------------------------------------------------------------------------
