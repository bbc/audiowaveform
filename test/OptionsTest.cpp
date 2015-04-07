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

#include "Options.h"
#include "Array.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

//------------------------------------------------------------------------------

using testing::EndsWith;
using testing::Eq;
using testing::HasSubstr;
using testing::MatchesRegex;
using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

class OptionsTest : public Test
{
    public:
        OptionsTest()
        {
        }

    protected:
        virtual void SetUp()
        {
            output.str(std::string());
            error.str(std::string());
        }

        virtual void TearDown()
        {
        }

        Options options_;
};

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReportErrorIfNoOptionsGiven)
{
    const char* const argv[] = { "appname" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_FALSE(result);

    std::string str = error.str();
    ASSERT_THAT(str, HasSubstr("appname"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnFilenamesWithLongArgs)
{
    const char* const argv[] = {
        "appname", "--input-filename", "test.mp3",
        "--output-filename", "test.dat"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getInputFilename(), StrEq("test.mp3"));
    ASSERT_THAT(options_.getOutputFilename(), StrEq("test.dat"));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnFilenamesWithShortArgs)
{
    const char* const argv[] = { "appname", "-i", "test.mp3", "-o", "test.dat" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getInputFilename(), StrEq("test.mp3"));
    ASSERT_THAT(options_.getOutputFilename(), StrEq("test.dat"));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingInputFilename)
{
    const char* const argv[] = { "appname", "-i", "-o", "test.dat" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_FALSE(result);

    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingOutputFilename)
{
    const char* const argv[] = { "appname", "-i", "test.mp3", "-o" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_FALSE(result);

    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnDefaultOptions)
{
    const char* const argv[] = { "appname", "-i", "test.mp3", "-o", "test.dat" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getStartTime(), Eq(0.0));
    ASSERT_FALSE(options_.hasEndTime());
    ASSERT_THAT(options_.getEndTime(), Eq(0.0));
    ASSERT_FALSE(options_.hasSamplesPerPixel());
    ASSERT_THAT(options_.getSamplesPerPixel(), Eq(256));
    ASSERT_THAT(options_.getBits(), Eq(16));
    ASSERT_THAT(options_.getImageWidth(), Eq(800));
    ASSERT_THAT(options_.getImageHeight(), Eq(250));
    ASSERT_TRUE(options_.getRenderAxisLabels());
    ASSERT_FALSE(options_.getHelp());
    ASSERT_FALSE(options_.getVersion());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnStartTimeWithLongArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--start", "1234.5"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getStartTime(), Eq(1234.5));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnStartTimeWithShortArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-s", "1234.5"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getStartTime(), Eq(1234.5));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidStartTime)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-s", "invalid"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingStartTime)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-s"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnEndTimeWithLongArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--end", "1234.5"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_TRUE(options_.hasEndTime());
    ASSERT_THAT(options_.getEndTime(), Eq(1234.5));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnEndTimeWithShortArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-e", "1234.5"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_TRUE(options_.hasEndTime());
    ASSERT_THAT(options_.getEndTime(), Eq(1234.5));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidEndTime)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-e", "invalid"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingEndTime)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-e"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(options_.hasEndTime());

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnDefaultEndTime)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_FALSE(options_.hasEndTime());
    ASSERT_THAT(options_.getEndTime(), Eq(0.0));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnWidthWithLongArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--width", "12345"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getImageWidth(), Eq(12345));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnWidthWithShortArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-w", "12345"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getImageWidth(), Eq(12345));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidWidth)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-w", "invalid"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingWidth)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-w"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnHeightWithLongArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--height", "23456"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getImageHeight(), Eq(23456));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnHeightWithShortArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-h", "23456"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getImageHeight(), Eq(23456));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidHeight)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-h", "invalid"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingHeight)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-h"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnZoomWithLongArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--zoom", "1000"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_TRUE(options_.hasSamplesPerPixel());
    ASSERT_THAT(options_.getSamplesPerPixel(), Eq(1000));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnZoomWithShortArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-z", "23456"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_TRUE(options_.hasSamplesPerPixel());
    ASSERT_THAT(options_.getSamplesPerPixel(), Eq(23456));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnDefaultZoomOption)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_FALSE(options_.hasSamplesPerPixel());
    ASSERT_THAT(options_.getSamplesPerPixel(), Eq(256));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidZoom)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-z", "invalid"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfZoomValueTooLarge)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-z", "2147483648" // INT_MAX + 1
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingZoom)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-z"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnPixelsPerSecond)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--pixels-per-second", "200"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_TRUE(options_.hasPixelsPerSecond());
    ASSERT_THAT(options_.getPixelsPerSecond(), Eq(200));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnDefaultPixelsPerSecondOption)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_FALSE(options_.hasPixelsPerSecond());
    ASSERT_THAT(options_.getPixelsPerSecond(), Eq(100));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidPixelsPerSecond)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--pixels-per-second", "invalid"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingPixelsPerSecond)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--pixels-per-second"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfPixelsPerSecondValueTooLarge)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--pixels-per-second", "2147483648" // INT_MAX + 1
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnBitsWithLongArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--bits", "8"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getBits(), Eq(8));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnBitsWithShortArg)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-b", "16"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);

    ASSERT_THAT(options_.getBits(), Eq(16));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfBitsInvalid)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "-b", "3"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);

    ASSERT_THAT(options_.getBits(), Eq(3));

    ASSERT_TRUE(output.str().empty());
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidBits)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--bits", "invalid"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfMissingBits)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--bits"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnDefaultBitsOption)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);
    ASSERT_TRUE(result);

    ASSERT_FALSE(options_.hasBits());
    ASSERT_THAT(options_.getBits(), Eq(16));

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisableAxisLabelRendering)
{
    const char* const argv[] = {
        "appname", "-i", "test.dat", "-o", "test.png", "--no-axis-labels"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());

    ASSERT_FALSE(options_.getRenderAxisLabels());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldEnableAxisLabelRendering)
{
    const char* const argv[] = {
        "appname", "-i", "test.dat", "-o", "test.png", "--with-axis-labels"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());

    ASSERT_TRUE(options_.getRenderAxisLabels());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldEnableAxisLabelRenderingByDefault)
{
    const char* const argv[] = {
        "appname", "-i", "test.dat", "-o", "test.png"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());

    ASSERT_TRUE(options_.getRenderAxisLabels());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnDefaultPngCompressionLevel)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());
    ASSERT_THAT(options_.getPngCompressionLevel(), Eq(-1));
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnPngCompressionLevel)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--compression", "9"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(error.str().empty());
    ASSERT_THAT(options_.getPngCompressionLevel(), Eq(9));
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfInvalidPngCompressionLevel)
{
    const char* const argv[] = {
        "appname", "-i", "test.mp3", "-o", "test.dat", "--compression", "10"
    };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnHelpFlag)
{
    const char* const argv[] = { "appname", "--help" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(options_.getHelp());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnVersionFlagWithLongArg)
{
    const char* const argv[] = { "appname", "--version" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(options_.getVersion());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldReturnVersionFlagWithShortArg)
{
    const char* const argv[] = { "appname", "-v" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_TRUE(result);
    ASSERT_TRUE(options_.getVersion());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfUnknownLongArg)
{
    const char* const argv[] = { "appname", "--unknown" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldDisplayErrorIfUnknownShortArg)
{
    const char* const argv[] = { "appname", "-u" };

    bool result = options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    ASSERT_FALSE(result);
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldOutputVersionInfo)
{
    std::ostringstream stream;
    options_.showVersion(stream);

    std::string str = stream.str();
    ASSERT_THAT(str, MatchesRegex("^AudioWaveform v[0-9]+\\.[0-9]+\\.[0-9]+\n$"));
}

//------------------------------------------------------------------------------

TEST_F(OptionsTest, shouldOutputUsageInfo)
{
    const char* const argv[] = { "appname" };

    // Must call parseCommandLine first...
    options_.parseCommandLine(ARRAY_LENGTH(argv), argv);

    std::ostringstream stream;
    options_.showUsage(stream);

    std::string str = stream.str();
    ASSERT_THAT(str, HasSubstr("appname"));
}

//------------------------------------------------------------------------------
