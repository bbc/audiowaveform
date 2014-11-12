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
using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

TEST(DurationScaleFactorTest, shouldThrowIfEndTimeLessThanStartTime)
{
    ASSERT_THROW(DurationScaleFactor(3.0, 2.0, 100), std::runtime_error);
}

//------------------------------------------------------------------------------

TEST(DurationScaleFactorTest, shouldThrowIfWidthIsZero)
{
    ASSERT_THROW(DurationScaleFactor(2.0, 3.0, 0), std::runtime_error);
}

//------------------------------------------------------------------------------

TEST(DurationScaleFactorTest, shouldThrowIfWidthIsNegative)
{
    ASSERT_THROW(DurationScaleFactor(2.0, 3.0, -1), std::runtime_error);
}

//------------------------------------------------------------------------------

TEST(PixelsPerSecondScaleFactorTest, shouldThrowIfZero)
{
    ASSERT_THROW(PixelsPerSecondScaleFactor(0), std::runtime_error);
}

//------------------------------------------------------------------------------

TEST(PixelsPerSecondScaleFactorTest, shouldThrowIfNegative)
{
    ASSERT_THROW(PixelsPerSecondScaleFactor(-1), std::runtime_error);
}

//------------------------------------------------------------------------------

TEST(PixelsPerSecondScaleFactorTest, shouldNotThrowIfPositive)
{
    ASSERT_NO_THROW(PixelsPerSecondScaleFactor(1));
}

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

TEST_F(WaveformGeneratorTest, shouldFailIfSamplesPerPixelIsNegative)
{
    WaveformBuffer buffer;
    SamplesPerPixelScaleFactor scale_factor(INT_MIN);
    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_FALSE(result);
    ASSERT_THAT(error.str(), StrEq("Invalid zoom: minimum 2\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldFailIfSamplesPerPixelIsZero)
{
    WaveformBuffer buffer;
    SamplesPerPixelScaleFactor scale_factor(0);
    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_FALSE(result);
    ASSERT_THAT(error.str(), StrEq("Invalid zoom: minimum 2\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldFailIfSamplesPerPixelIsOne)
{
    WaveformBuffer buffer;
    SamplesPerPixelScaleFactor scale_factor(1);
    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_FALSE(result);
    ASSERT_THAT(error.str(), StrEq("Invalid zoom: minimum 2\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldSucceedIfSamplesPerPixelIsTwo)
{
    WaveformBuffer buffer;
    SamplesPerPixelScaleFactor scale_factor(2);
    WaveformGenerator generator(buffer,scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldSucceedIfSamplesPerPixelIsLarge)
{
    WaveformBuffer buffer;
    SamplesPerPixelScaleFactor scale_factor(INT_MAX);
    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldSucceedIfEndTimeGreaterThanStartTime)
{
    WaveformBuffer buffer;
    DurationScaleFactor scale_factor(2.0, 3.0, 100);

    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());

    ASSERT_THAT(generator.getSamplesPerPixel(), Eq(441));
    ASSERT_THAT(buffer.getSampleRate(), Eq(44100));
    ASSERT_THAT(buffer.getSamplesPerPixel(), Eq(441));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldFailIfSamplesPerPixelIsTooSmall)
{
    WaveformBuffer buffer;
    DurationScaleFactor scale_factor(2.0, 2.1, 2500);

    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_FALSE(result);
    ASSERT_THAT(error.str(), StrEq("Invalid zoom: minimum 2\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldSetBufferAttributes)
{
    WaveformBuffer buffer;

    const int samples_per_pixel = 300;

    SamplesPerPixelScaleFactor scale_factor(samples_per_pixel);
    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());

    ASSERT_THAT(buffer.getSampleRate(), Eq(44100));
    ASSERT_THAT(buffer.getSamplesPerPixel(), Eq(300));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldComputeMaxAndMinValuesFromStereoInput)
{
    WaveformBuffer buffer;

    const int samples_per_pixel = 300;

    SamplesPerPixelScaleFactor scale_factor(samples_per_pixel);
    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 2;
    const int BUFFER_SIZE = 1024;

    short samples[BUFFER_SIZE];
    memset(samples, 0, sizeof(samples));

    const int frames = BUFFER_SIZE / channels;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());

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

    // Check min and max values are average of left and right channels
    ASSERT_THAT(buffer.getMinSample(0), Eq(-101));
    ASSERT_THAT(buffer.getMaxSample(0), Eq(101));

    ASSERT_THAT(buffer.getMinSample(1), Eq(-201));
    ASSERT_THAT(buffer.getMaxSample(1), Eq(201));
}

//------------------------------------------------------------------------------

TEST_F(WaveformGeneratorTest, shouldComputeMaxAndMinValuesFromMonoInput)
{
    WaveformBuffer buffer;

    const int samples_per_pixel = 300;

    SamplesPerPixelScaleFactor scale_factor(samples_per_pixel);
    WaveformGenerator generator(buffer, scale_factor);

    const int sample_rate = 44100;
    const int channels    = 1;
    const int BUFFER_SIZE = 512;

    short samples[BUFFER_SIZE];
    memset(samples, 0, sizeof(samples));

    const int frames = BUFFER_SIZE / channels;

    bool result = generator.init(sample_rate, channels, BUFFER_SIZE);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());

    // samples for first waveform data point
    samples[0] = 100;
    samples[100] = 98;
    samples[200] = -98;
    samples[299] = -102;

    // samples for second waveform data point
    samples[300] = 197;
    samples[400] = -200;
    samples[450] = -197;
    samples[511] = 202;

    result = generator.process(samples, frames);
    ASSERT_TRUE(result);

    generator.done();

    // Check contents of buffer
    ASSERT_THAT(buffer.getSampleRate(), Eq(44100));
    ASSERT_THAT(buffer.getSamplesPerPixel(), Eq(300));
    ASSERT_THAT(buffer.getSize(), Eq(2)); // 512 / 300 = 1 remainder 212
                                          // => 2 output points total

    // Check min and max values
    ASSERT_THAT(buffer.getMinSample(0), Eq(-102));
    ASSERT_THAT(buffer.getMaxSample(0), Eq(100));

    ASSERT_THAT(buffer.getMinSample(1), Eq(-200));
    ASSERT_THAT(buffer.getMaxSample(1), Eq(202));
}

//------------------------------------------------------------------------------
