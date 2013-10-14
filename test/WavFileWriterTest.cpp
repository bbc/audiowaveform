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

#include "WavFileWriter.h"
#include "util/FileDeleter.h"
#include "util/Streams.h"
#include "util/TempFilename.h"

#include "gmock/gmock.h"

#include <boost/filesystem.hpp>

#include <sys/stat.h>

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

        TempFilename temp_filename_;
};

//------------------------------------------------------------------------------

TEST_F(WavFileWriterTest, shouldCreateEmptyWavFile)
{
    const char* filename = temp_filename_.getFilename();
    ASSERT_NE(nullptr, filename);

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    WavFileWriter writer(filename);

    const int sample_rate = 44100;
    const int channels    = 1;
    const int BUFFER_SIZE = 1024;

    bool success = writer.init(sample_rate, channels, BUFFER_SIZE);
    ASSERT_TRUE(success);

    writer.done();

    // Check file was created.
    struct stat info;
    memset(&info, 0, sizeof(info));

    int result = stat(filename, &info);
    ASSERT_EQ(0, result);

    // Check no error reported.
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WavFileWriterTest, shouldCreateMonoWavFile)
{
    const char* filename = temp_filename_.getFilename();
    ASSERT_NE(nullptr, filename);

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(filename);

    WavFileWriter writer(filename);

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

    // Check file was created.
    struct stat info;
    memset(&info, 0, sizeof(info));

    int result = stat(filename, &info);
    ASSERT_EQ(0, result);

    // Check file size: 44 byte WAV header + 1024 * 2 bytes waveform data
    ASSERT_EQ(44 + 1024 * 2, info.st_size);

    // Check no error reported.
    ASSERT_TRUE(error.str().empty());

    ASSERT_FALSE(output.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(WavFileWriterTest, shouldReportErrorIfUnableToCreateFile)
{
    // Attempt to create wav file in a directory that does not exist.
    const char* filename = temp_filename_.getFilename();
    ASSERT_NE(nullptr, filename);

    boost::filesystem::path path(filename);
    path /= "test.wav";

    WavFileWriter writer(path.c_str());

    const int sample_rate = 44100;
    const int channels    = 1;
    const int BUFFER_SIZE = 1024;

    bool success = writer.init(sample_rate, channels, BUFFER_SIZE);
    ASSERT_FALSE(success);

    // Check error is reported.
    ASSERT_FALSE(error.str().empty());
}

//------------------------------------------------------------------------------
