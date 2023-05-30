//------------------------------------------------------------------------------
//
// Copyright 2013-2023 BBC Research and Development
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

#include "GdImageRenderer.h"
#include "WaveformBuffer.h"
#include "WaveformColors.h"
#include "util/FileDeleter.h"
#include "util/FileUtil.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

//------------------------------------------------------------------------------

using testing::EndsWith;
using testing::Eq;
using testing::Gt;
using testing::MatchesRegex;
using testing::StartsWith;
using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

class GdImageRendererTest : public Test
{
    protected:
        virtual void SetUp()
        {
            output.str(std::string());
            error.str(std::string());
        }

        virtual void TearDown()
        {
        }

        void testImageRendering(
            bool axis_labels,
            bool bars,
            bool rounded,
            const std::string& expected_output
        );

        WaveformBuffer buffer_;
        GdImageRenderer renderer_;
};

//------------------------------------------------------------------------------

void GdImageRendererTest::testImageRendering(
    bool axis_labels,
    bool bars,
    bool rounded,
    const std::string& expected_output)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".png");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    WaveformBuffer buffer;
    bool result = buffer.load("../test/data/test_file_stereo_8bit_64spp_wav.dat");
    ASSERT_TRUE(result);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    if (bars) {
        renderer.setBarStyle(8, 4, rounded);
    }

    renderer.setStartTime(5.0);
    renderer.enableAxisLabels(axis_labels);

    result = renderer.create(buffer, 1000, 300, colors); // zoom: 128
    ASSERT_TRUE(result);

    result = renderer.saveAsPng(filename.c_str());
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Gt(0U));

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StartsWith(expected_output));
    ASSERT_THAT(error.str(), EndsWith(".png\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithAxisLabels)
{
    std::string expected_output(
        "Input file: ../test/data/test_file_stereo_8bit_64spp_wav.dat\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 8\n"
        "Samples per pixel: 64\n"
        "Length: 1774 points\n"
        "Image dimensions: 1000x300 pixels\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Samples per pixel: 64\n"
        "Start time: 5 seconds\n"
        "Start index: 1250\n"
        "Buffer size: 1774\n"
        "Axis labels: yes\n"
        "Waveform style: normal\n"
        "Amplitude scale: 1\n"
        "Output file: "
    );

    testImageRendering(true, false, false, expected_output);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithoutAxisLabels)
{
    std::string expected_output(
        "Input file: ../test/data/test_file_stereo_8bit_64spp_wav.dat\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 8\n"
        "Samples per pixel: 64\n"
        "Length: 1774 points\n"
        "Image dimensions: 1000x300 pixels\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Samples per pixel: 64\n"
        "Start time: 5 seconds\n"
        "Start index: 1250\n"
        "Buffer size: 1774\n"
        "Axis labels: no\n"
        "Waveform style: normal\n"
        "Amplitude scale: 1\n"
        "Output file: "
    );

    testImageRendering(false, false, false, expected_output);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithSquareBars)
{
    std::string expected_output(
        "Input file: ../test/data/test_file_stereo_8bit_64spp_wav.dat\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 8\n"
        "Samples per pixel: 64\n"
        "Length: 1774 points\n"
        "Image dimensions: 1000x300 pixels\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Samples per pixel: 64\n"
        "Start time: 5 seconds\n"
        "Start index: 1250\n"
        "Buffer size: 1774\n"
        "Axis labels: yes\n"
        "Waveform style: bars\n"
        "Bar width: 8\n"
        "Bar gap: 4\n"
        "Bar style: square\n"
        "Amplitude scale: 1\n"
        "Output file: "
    );

    testImageRendering(true, true, false, expected_output);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithRoundedBars)
{
    std::string expected_output(
        "Input file: ../test/data/test_file_stereo_8bit_64spp_wav.dat\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 8\n"
        "Samples per pixel: 64\n"
        "Length: 1774 points\n"
        "Image dimensions: 1000x300 pixels\n"
        "Channels: 1\n"
        "Sample rate: 16000 Hz\n"
        "Samples per pixel: 64\n"
        "Start time: 5 seconds\n"
        "Start index: 1250\n"
        "Buffer size: 1774\n"
        "Axis labels: yes\n"
        "Waveform style: bars\n"
        "Bar width: 8\n"
        "Bar gap: 4\n"
        "Bar style: rounded\n"
        "Amplitude scale: 1\n"
        "Output file: "
    );

    testImageRendering(true, true, true, expected_output);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfImageWidthIsLessThanMinimum)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(48000);
    buffer.setSamplesPerPixel(64);
    buffer.appendSamples(-1, 1);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    bool result = renderer.create(buffer, 0, 300, colors);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    ASSERT_THAT(error.str(), StrEq("Invalid image width: minimum 1\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfImageHeightIsLessThanMinimum)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(48000);
    buffer.setSamplesPerPixel(64);
    buffer.appendSamples(-1, 1);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    bool result = renderer.create(buffer, 800, 0, colors);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    ASSERT_THAT(error.str(), StrEq("Invalid image height: minimum 1\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfSampleRateIsZero)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(0);
    buffer.setSamplesPerPixel(64);
    buffer.appendSamples(-1, 1);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    bool result = renderer.create(buffer, 800, 250, colors);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    ASSERT_THAT(error.str(), StartsWith("Invalid sample rate: 0 Hz\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfSampleRateIsNegative)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(-1);
    buffer.setSamplesPerPixel(64);
    buffer.appendSamples(-1, 1);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    bool result = renderer.create(buffer, 800, 250, colors);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    ASSERT_THAT(error.str(), StrEq("Invalid sample rate: -1 Hz\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfScaleIsZero)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(44100);
    buffer.setSamplesPerPixel(0);
    buffer.appendSamples(-1, 1);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    bool result = renderer.create(buffer, 800, 250, colors);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    ASSERT_THAT(error.str(), StrEq("Invalid waveform scale: 0\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfScaleIsNegative)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(44100);
    buffer.setSamplesPerPixel(-1);
    buffer.appendSamples(-1, 1);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    bool result = renderer.create(buffer, 800, 250, colors);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    ASSERT_THAT(error.str(), StartsWith("Invalid waveform scale: -1\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfWaveformBufferIsEmpty)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(44100);
    buffer.setSamplesPerPixel(64);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;

    bool result = renderer.create(buffer, 800, 250, colors);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    ASSERT_THAT(error.str(), StrEq("Empty waveform buffer\n"));
}

//------------------------------------------------------------------------------
