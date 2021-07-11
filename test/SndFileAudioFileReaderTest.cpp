//------------------------------------------------------------------------------
//
// Copyright 2013-2021 BBC Research and Development
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

#include "SndFileAudioFileReader.h"
#include "mocks/MockAudioProcessor.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

#include <boost/filesystem.hpp>

#include <sstream>

//------------------------------------------------------------------------------

using testing::_;
using testing::AnyOf;
using testing::EndsWith;
using testing::Eq;
using testing::HasSubstr;
using testing::Gt;
using testing::InSequence;
using testing::Return;
using testing::StartsWith;
using testing::StrEq;
using testing::StrictMock;
using testing::Test;

//------------------------------------------------------------------------------

class SndFileAudioFileReaderTest : public Test
{
    public:
        SndFileAudioFileReaderTest()
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

        SndFileAudioFileReader reader_;
};

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldOpenWavFile)
{
    bool result = reader_.open("../test/data/test_file_stereo.wav");
    ASSERT_TRUE(result);

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StrEq(
        "Input file: ../test/data/test_file_stereo.wav\n"
        "Frames: 113519\n"
        "Sample rate: 16000 Hz\n"
        "Channels: 2\n"
        "Format: 0x10002\n"
        "Sections: 1\n"
        "Seekable: yes\n"
    ));
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldOpenFlacFile)
{
    bool result = reader_.open("../test/data/test_file_stereo.flac");
    ASSERT_TRUE(result);

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StrEq(
        "Input file: ../test/data/test_file_stereo.flac\n"
        "Frames: 113519\n"
        "Sample rate: 16000 Hz\n"
        "Channels: 2\n"
        "Format: 0x170002\n"
        "Sections: 1\n"
        "Seekable: yes\n"
    ));
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldReportErrorIfFileNotFound)
{
    bool result = reader_.open("../test/data/unknown.wav");
    ASSERT_FALSE(result);

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StartsWith(
        "Failed to read file: ../test/data/unknown.wav\n"
    ));
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldFailToProcessIfFileNotOpen)
{
    StrictMock<MockAudioProcessor> processor;

    EXPECT_CALL(processor, init(_, _, _, _)).Times(0);
    EXPECT_CALL(processor, shouldContinue()).Times(0);
    EXPECT_CALL(processor, process(_, _)).Times(0);
    EXPECT_CALL(processor, done()).Times(0);

    bool result = reader_.run(processor);
    ASSERT_FALSE(result);

    // No error message expected.
    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StrEq(""));
}

//------------------------------------------------------------------------------

static void testProcessStereo(const std::string& filename, const std::string& format)
{
    boost::filesystem::path path = "../test/data";
    path /= filename;

    SndFileAudioFileReader reader;

    ASSERT_NO_THROW(reader.open(path.c_str()));

    StrictMock<MockAudioProcessor> processor;

    InSequence sequence; // Calls expected in the order listed below.

    EXPECT_CALL(processor, init(16000, 2, 113519, 16384)).WillOnce(Return(true));
    EXPECT_CALL(processor, shouldContinue()).WillOnce(Return(true));

    // Total number of frames: 113519, 13 x 8192 frames then 1 x 7023
    EXPECT_CALL(processor, process(_, 8192)).Times(13).WillRepeatedly(Return(true));
    EXPECT_CALL(processor, process(_, 7023)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(processor, done());

    bool result = reader.run(processor);
    ASSERT_TRUE(result);

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StartsWith(
        "Input file: " + path.string() + "\n"
        "Frames: 113519\n"
        "Sample rate: 16000 Hz\n"
        "Channels: 2\n"
        "Format: " + format + "\n"
        "Sections: 1\n"
        "Seekable: yes\n"
    ));
    ASSERT_THAT(error.str(), EndsWith(
        "Read 113519 frames\n"
    ));
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldProcessStereoWavFile)
{
    testProcessStereo("test_file_stereo.wav", "0x10002");
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldProcessStereoFlacFile)
{
    testProcessStereo("test_file_stereo.flac", "0x170002");
}

//------------------------------------------------------------------------------

static void testProcessMono(const std::string& filename, const std::string& format)
{
    boost::filesystem::path path = "../test/data";
    path /= filename;

    SndFileAudioFileReader reader;

    ASSERT_NO_THROW(reader.open(path.c_str()));

    StrictMock<MockAudioProcessor> processor;

    InSequence sequence; // Calls expected in the order listed below.

    EXPECT_CALL(processor, init(16000, 1, 113519, 16384)).WillOnce(Return(true));
    EXPECT_CALL(processor, shouldContinue()).WillOnce(Return(true));

    // Total number of frames: 113519, 6 x 16384 frames then 1 x 15215
    EXPECT_CALL(processor, process(_, 16384)).Times(6).WillRepeatedly(Return(true));
    EXPECT_CALL(processor, process(_, 15215)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(processor, done());

    bool result = reader.run(processor);
    ASSERT_TRUE(result);

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StartsWith(
        "Input file: " + path.string() + "\n"
        "Frames: 113519\n"
        "Sample rate: 16000 Hz\n"
        "Channels: 1\n"
        "Format: " + format + "\n"
        "Sections: 1\n"
        "Seekable: yes\n"
    ));
    ASSERT_THAT(error.str(), EndsWith(
        "Read 113519 frames\n"
    ));
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldProcessMonoWavFile)
{
    testProcessMono("test_file_mono.wav", "0x10002");
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldProcessMonoFlacFile)
{
    testProcessMono("test_file_mono.flac", "0x170002");
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldNotProcessFileMoreThanOnce)
{
    ASSERT_NO_THROW(reader_.open("../test/data/test_file_stereo.wav"));

    StrictMock<MockAudioProcessor> processor;

    InSequence sequence; // Calls expected in the order listed below.

    EXPECT_CALL(processor, init(16000, 2, 113519, 16384)).WillOnce(Return(true));
    EXPECT_CALL(processor, shouldContinue()).WillOnce(Return(true));

    // Total number of frames: 113519, 13 x 8192 frames then 1 x 7023
    EXPECT_CALL(processor, process(_, 8192)).Times(13).WillRepeatedly(Return(true));
    EXPECT_CALL(processor, process(_, 7023)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(processor, done()).Times(1);

    bool result = reader_.run(processor);

    ASSERT_TRUE(result);

    // Attempting to process the file again should fail
    error.str(std::string());

    result = reader_.run(processor);
    ASSERT_FALSE(result);

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), StrEq(""));
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldReportErrorIfNotAWavFile)
{
    const char* filename = "../test/data/test_file_stereo.mp3";

    bool result = reader_.open(filename);
    ASSERT_FALSE(result);

    ASSERT_THAT(output.str(), StrEq(""));
    ASSERT_THAT(error.str(), AnyOf(
        StrEq(
            "Failed to read file: ../test/data/test_file_stereo.mp3\n"
            "File contains data in an unknown format.\n"
        ),
        StrEq(
            "Failed to read file: ../test/data/test_file_stereo.mp3\n"
            "Format not recognised.\n"
        )
    ));
}

//------------------------------------------------------------------------------
