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
#include "util/FileDeleter.h"
#include "util/FileUtil.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

#include <fstream>

//------------------------------------------------------------------------------

using testing::EndsWith;
using testing::Eq;
using testing::Gt;
using testing::HasSubstr;
using testing::Ne;
using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

class WaveformBufferTest : public Test
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

        WaveformBuffer buffer_;
};

//------------------------------------------------------------------------------

class WaveformBufferSaveTest : public WaveformBufferTest
{
    protected:
        virtual void SetUp()
        {
        }

        virtual void TearDown()
        {
        }

        WaveformBuffer buffer_;
};

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldConstructWithDefaultState)
{
    ASSERT_THAT(buffer_.getSampleRate(), Eq(0));
    ASSERT_THAT(buffer_.getSamplesPerPixel(), Eq(0));
    ASSERT_THAT(buffer_.getSize(), Eq(0));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldLoadValid16BitDataFile)
{
    bool result = buffer_.load("../test/data/test_file_stereo_16bit_64spp.dat");
    ASSERT_TRUE(result);

    ASSERT_THAT(buffer_.getSampleRate(), Eq(16000));
    ASSERT_THAT(buffer_.getSamplesPerPixel(), Eq(64));
    ASSERT_THAT(buffer_.getSize(), Eq(1800));

    std::string expected_output(
        "Reading waveform data file: ../test/data/test_file_stereo_16bit_64spp.dat\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 16\n"
        "Samples per pixel: 64\n"
        "Length: 1800 points\n"
    );

    ASSERT_THAT(output.str(), StrEq(expected_output));
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldLoadValid8BitDataFile)
{
    bool result = buffer_.load("../test/data/test_file_stereo_8bit_64spp.dat");
    ASSERT_TRUE(result);

    ASSERT_THAT(buffer_.getSampleRate(), Eq(16000));
    ASSERT_THAT(buffer_.getSamplesPerPixel(), Eq(64));
    ASSERT_THAT(buffer_.getSize(), Eq(1800));

    std::string expected_output(
        "Reading waveform data file: ../test/data/test_file_stereo_8bit_64spp.dat\n"
        "Sample rate: 16000 Hz\n"
        "Bits: 8\n"
        "Samples per pixel: 64\n"
        "Length: 1800 points\n"
    );

    ASSERT_THAT(output.str(), StrEq(expected_output));
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldNotLoadDataFileIfNotVersion1)
{
    const char* filename = "../test/data/version2.dat";

    bool result = buffer_.load(filename);
    ASSERT_FALSE(result);

    std::string str = output.str();
    ASSERT_THAT(str, HasSubstr(filename));

    str = error.str();
    ASSERT_THAT(str, HasSubstr(filename));
    ASSERT_THAT(str, HasSubstr("Cannot load data file version: 2"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldReportErrorIfSizeMismatch)
{
    const char* filename = "../test/data/size_mismatch.dat";

    bool result = buffer_.load(filename);
    ASSERT_TRUE(result);

    std::string str = output.str();
    ASSERT_THAT(str, HasSubstr(filename));

    str = error.str();
    ASSERT_THAT(str, StrEq("Expected 2056 points, read 1800 min and max points\n"));

    ASSERT_THAT(buffer_.getSize(), Eq(1800)); // Actual size loaded
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldReportErrorIfFileNotFound)
{
    const char* filename = "../test/data/unknown.dat";

    bool result = buffer_.load(filename);
    ASSERT_FALSE(result);

    ASSERT_TRUE(output.str().empty());

    std::string str = error.str();
    ASSERT_THAT(str, HasSubstr("No such file or directory"));
    ASSERT_THAT(str, HasSubstr(filename));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldNotLoadDataFileWithSampleRateBelowMinimum)
{
    const char* filename = "../test/data/sample_rate_too_low.dat";

    bool result = buffer_.load(filename);
    ASSERT_FALSE(result);

    std::string str = error.str();
    ASSERT_THAT(str, HasSubstr(filename));
    ASSERT_THAT(str, HasSubstr("Invalid sample rate"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldNotLoadDataFileWithSamplesPerPixelBelowMinimum)
{
    const char* filename = "../test/data/samples_per_pixel_too_low.dat";

    bool result = buffer_.load(filename);
    ASSERT_FALSE(result);

    std::string str = error.str();
    ASSERT_THAT(str, HasSubstr(filename));
    ASSERT_THAT(str, HasSubstr("Invalid samples per pixel"));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferTest, shouldLoadDataFileIfSizeIsZero)
{
    bool result = buffer_.load("../test/data/zero_length.dat");
    ASSERT_TRUE(result);

    ASSERT_THAT(buffer_.getSampleRate(), Eq(16000));
    ASSERT_THAT(buffer_.getSamplesPerPixel(), Eq(64));
    ASSERT_THAT(buffer_.getSize(), Eq(0));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldSaveEmptyDataFile)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    bool result = buffer_.save(filename.c_str());
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Eq(20U));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldSave16BitDataFile)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    buffer_.setSampleRate(44100);
    buffer_.setSamplesPerPixel(256);

    buffer_.appendSamples(-1000, 1000);

    bool result = buffer_.save(filename.c_str(), 16);
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Eq(24U)); // 20 byte header + 4 bytes data
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldSave8BitDataFile)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    buffer_.setSampleRate(44100);
    buffer_.setSamplesPerPixel(256);

    buffer_.appendSamples(-100, 100);

    bool result = buffer_.save(filename.c_str(), 8);
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Eq(22U)); // 20 byte header + 2 bytes data
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldReportErrorIfNot8Or16Bits)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    bool result = buffer_.save(filename.c_str(), 10);
    ASSERT_FALSE(result);

    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldSave16BitTextFile)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    buffer_.setSampleRate(44100);
    buffer_.setSamplesPerPixel(256);

    buffer_.appendSamples(-1024, 1024);
    buffer_.appendSamples(-2048, 2048);

    bool result = buffer_.saveAsText(filename.c_str());
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Gt(0U));

    const std::string data = FileUtil::readTextFile(filename.c_str());
    ASSERT_THAT(data, StrEq("-1024,1024\n-2048,2048\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldSave8BitTextFile)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    buffer_.setSampleRate(44100);
    buffer_.setSamplesPerPixel(256);

    buffer_.appendSamples(-1024, 1024);
    buffer_.appendSamples(-2048, 2048);

    bool result = buffer_.saveAsText(filename.c_str(), 8);
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Gt(0U));

    const std::string data = FileUtil::readTextFile(filename);
    ASSERT_THAT(data, StrEq("-4,4\n-8,8\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldSave16BitJsonFile)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    buffer_.setSampleRate(44100);
    buffer_.setSamplesPerPixel(256);

    buffer_.appendSamples(-1024, 1024);
    buffer_.appendSamples(-2048, 2048);

    bool result = buffer_.saveAsJson(filename.c_str());
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Gt(0U));

    const std::string data = FileUtil::readTextFile(filename);
    ASSERT_THAT(data, StrEq("{\"sample_rate\":44100,\"samples_per_pixel\":256,\"bits\":16,\"length\":2,\"data\":[-1024,1024,-2048,2048]}\n"));
}

//------------------------------------------------------------------------------

TEST_F(WaveformBufferSaveTest, shouldSave8BitJsonFile)
{
    const boost::filesystem::path filename = FileUtil::getTempFilename(".dat");

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    buffer_.setSampleRate(44100);
    buffer_.setSamplesPerPixel(256);

    buffer_.appendSamples(-1024, 1024);
    buffer_.appendSamples(-2048, 2048);

    bool result = buffer_.saveAsJson(filename.c_str(), 8);
    ASSERT_TRUE(result);

    // Check file was created.
    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    ASSERT_THAT(error_code, Eq(boost::system::errc::success));
    ASSERT_THAT(size, Gt(0U));

    const std::string data = FileUtil::readTextFile(filename);
    ASSERT_THAT(data, StrEq("{\"sample_rate\":44100,\"samples_per_pixel\":256,\"bits\":8,\"length\":2,\"data\":[-4,4,-8,8]}\n"));
}

//------------------------------------------------------------------------------
