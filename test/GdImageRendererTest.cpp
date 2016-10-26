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
};

//------------------------------------------------------------------------------

static void testImageRendering(bool axis_labels, const std::string& expected_output)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".png");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    WaveformBuffer buffer;
    bool result = buffer.load("../test/data/test_file_stereo_8bit_64spp.dat");
    ASSERT_TRUE(result);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;
    result = renderer.create(buffer, 5.0, 1000, 300, colors, axis_labels, false, 1.0); // zoom: 128
    ASSERT_TRUE(result);

    result = renderer.saveAsPng(filename.c_str());
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Gt(0U));

    ASSERT_THAT(output.str(), MatchesRegex(expected_output));
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithAxisLabels)
{
    std::string expected_output(
        "Reading waveform data file: \\.\\./test/data/test_file_stereo_8bit_64spp\\.dat\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 8\n"
        "Samples per pixel: 64\n"
        "Length: 1800 points\n"
        "Image dimensions: 1000x300 pixels\n"
        "Sample rate: 16000 Hz\n"
        "Samples per pixel: 64\n"
        "Start time: 5 seconds\n"
        "Start index: 1250\n"
        "Buffer size: 1800\n"
        "Axis labels: yes\n"
        "Amplitude scale: 1\n"
        "Writing PNG file: .*\n"
    );

    testImageRendering(true, expected_output);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithoutAxisLabels)
{
    std::string expected_output(
        "Reading waveform data file: \\.\\./test/data/test_file_stereo_8bit_64spp\\.dat\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 8\n"
        "Samples per pixel: 64\n"
        "Length: 1800 points\n"
        "Image dimensions: 1000x300 pixels\n"
        "Sample rate: 16000 Hz\n"
        "Samples per pixel: 64\n"
        "Start time: 5 seconds\n"
        "Start index: 1250\n"
        "Buffer size: 1800\n"
        "Axis labels: no\n"
        "Amplitude scale: 1\n"
        "Writing PNG file: .*\n"
    );

    testImageRendering(false, expected_output);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfImageWidthIsLessThanMinimum)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(48000);
    buffer.setSamplesPerPixel(64);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;
    bool result = renderer.create(buffer, 5.0, 0, 300, colors, true, false, 1.0);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid image width"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfImageHeightIsLessThanMinimum)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(48000);
    buffer.setSamplesPerPixel(64);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;
    bool result = renderer.create(buffer, 5.0, 800, 0, colors, true, false, 1.0);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid image height"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfSampleRateIsTooHigh)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(50001);
    buffer.setSamplesPerPixel(64);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;
    bool result = renderer.create(buffer, 5.0, 800, 250, colors, true, false, 1.0);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid sample rate"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfScaleIsTooHigh)
{
    WaveformBuffer buffer;
    buffer.setSampleRate(50000);
    buffer.setSamplesPerPixel(2000001);

    const WaveformColors& colors = audacity_waveform_colors;

    GdImageRenderer renderer;
    bool result = renderer.create(buffer, 5.0, 800, 250, colors, true, false, 1.0);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid zoom"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------
