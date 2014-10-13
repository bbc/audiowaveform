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
using testing::StartsWith;
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

        void testImageRendering(bool axis_labels);

        WaveformBuffer buffer_;
        GdImageRenderer renderer_;
};

//------------------------------------------------------------------------------

void GdImageRendererTest::testImageRendering(bool axis_labels)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".png");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    bool result = buffer_.load("../test/data/test_file_stereo_8bit_64spp.dat");
    ASSERT_TRUE(result);

    const WaveformColors& colors = audacity_waveform_colors;

    result = renderer_.create(buffer_, 5.0, 1000, 300, colors, axis_labels); // zoom: 128
    ASSERT_TRUE(result);

    result = renderer_.saveAsPng(filename.c_str());
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Gt(0U));

    ASSERT_FALSE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithAxisLabels)
{
    testImageRendering(true);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldRenderImageWithoutAxisLabels)
{
    testImageRendering(false);
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfImageWidthIsLessThanMinimum)
{
    buffer_.setSampleRate(48000);
    buffer_.setSamplesPerPixel(64);

    const WaveformColors& colors = audacity_waveform_colors;

    bool result = renderer_.create(buffer_, 5.0, 0, 300, colors, true);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid image width"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfImageHeightIsLessThanMinimum)
{
    buffer_.setSampleRate(48000);
    buffer_.setSamplesPerPixel(64);

    const WaveformColors& colors = audacity_waveform_colors;

    bool result = renderer_.create(buffer_, 5.0, 800, 0, colors, true);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid image height"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfSampleRateIsTooHigh)
{
    buffer_.setSampleRate(50001);
    buffer_.setSamplesPerPixel(64);

    const WaveformColors& colors = audacity_waveform_colors;

    bool result = renderer_.create(buffer_, 5.0, 800, 250, colors, true);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid sample rate"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(GdImageRendererTest, shouldReportErrorIfScaleIsTooHigh)
{
    buffer_.setSampleRate(50000);
    buffer_.setSamplesPerPixel(2000001);

    const WaveformColors& colors = audacity_waveform_colors;

    bool result = renderer_.create(buffer_, 5.0, 800, 250, colors, true);

    ASSERT_FALSE(result);
    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, StartsWith("Invalid zoom"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------
