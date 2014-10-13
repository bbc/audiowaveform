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

#include "WavFileWriter.h"
#include "util/FileDeleter.h"
#include "util/FileUtil.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

#include <boost/filesystem.hpp>

//------------------------------------------------------------------------------

using testing::Eq;

//------------------------------------------------------------------------------

class WavFileWriterTest : public ::testing::Test
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

TEST_F(WavFileWriterTest, shouldCreateEmptyWavFile)
{
    boost::filesystem::path filename = FileUtil::getTempFilename();

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    WavFileWriter writer(filename.c_str());

    const int sample_rate = 44100;
    const int channels    = 1;
    const int BUFFER_SIZE = 1024;

    bool success = writer.init(sample_rate, channels, BUFFER_SIZE);
    ASSERT_TRUE(success);

    writer.done();

    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    // Check file was created.
    ASSERT_THAT(error_code, Eq(boost::system::errc::success));

    // Check file size: 44 byte WAV header
    ASSERT_THAT(size, Eq(44U));

    // Check no error reported.
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WavFileWriterTest, shouldCreateMonoWavFile)
{
    boost::filesystem::path filename = FileUtil::getTempFilename();

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    WavFileWriter writer(filename.c_str());

    const int sample_rate = 44100;
    const int channels    = 1;
    const int BUFFER_SIZE = 1024;

    bool success = writer.init(sample_rate, channels, BUFFER_SIZE);
    ASSERT_TRUE(success);

    short samples[BUFFER_SIZE];
    memset(samples, 0, sizeof(samples));

    const int frames = BUFFER_SIZE;

    success = writer.process(samples, frames);
    ASSERT_TRUE(success);

    writer.done();

    boost::system::error_code error_code;
    boost::uintmax_t size = boost::filesystem::file_size(filename, error_code);

    // Check file was created.
    ASSERT_THAT(error_code, Eq(boost::system::errc::success));

    // Check file size: 44 byte WAV header + 1024 * 2 bytes waveform data
    ASSERT_THAT(size, Eq(44U + 1024 * 2));

    // Check no error reported.
    ASSERT_TRUE(error.str().empty());

    ASSERT_FALSE(output.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WavFileWriterTest, shouldReportErrorIfUnableToCreateFile)
{
    // Attempt to create wav file in a directory that does not exist.
    boost::filesystem::path filename = FileUtil::getTempFilename();
    filename /= "test.wav";

    WavFileWriter writer(filename.c_str());

    const int sample_rate = 44100;
    const int channels    = 1;
    const int BUFFER_SIZE = 1024;

    bool success = writer.init(sample_rate, channels, BUFFER_SIZE);
    ASSERT_FALSE(success);

    // Check error is reported.
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------
