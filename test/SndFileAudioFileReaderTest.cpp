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

#include "SndFileAudioFileReader.h"
#include "mocks/MockAudioProcessor.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

#include <sstream>

//------------------------------------------------------------------------------

using testing::_;
using testing::EndsWith;
using testing::Eq;
using testing::HasSubstr;
using testing::Gt;
using testing::InSequence;
using testing::Return;
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
    ASSERT_FALSE(output.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldReportErrorIfFileNotFound)
{
    const char* filename = "../test/data/unknown.wav";

    bool result = reader_.open(filename);

    ASSERT_FALSE(result);

    std::string str = error.str();
    ASSERT_THAT(str, HasSubstr(filename));
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldFailToProcessIfFileNotOpen)
{
    StrictMock<MockAudioProcessor> processor;

    EXPECT_CALL(processor, init(_, _, _)).Times(0);
    EXPECT_CALL(processor, process(_, _)).Times(0);
    EXPECT_CALL(processor, done()).Times(0);

    bool result = reader_.run(processor);
    ASSERT_FALSE(result);

    // No error message expected.
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldProcessStereoWavFile)
{
    bool result = reader_.open("../test/data/test_file_stereo.wav");
    ASSERT_TRUE(result);

    StrictMock<MockAudioProcessor> processor;

    InSequence sequence; // Calls expected in the order listed below.

    EXPECT_CALL(processor, init(16000, 2, 16384)).WillOnce(Return(true));

    // Total number of frames: 114624, 13 x 8192 frames then 1 x 8128
    EXPECT_CALL(processor, process(_, 8192)).Times(13).WillRepeatedly(Return(true));
    EXPECT_CALL(processor, process(_, 8128)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(processor, done());

    result = reader_.run(processor);

    std::string expected_output(
        "Input file: ../test/data/test_file_stereo.wav\n"
        "Frames: 114624\n"
        "Sample rate: 16000 Hz\n"
        "Channels: 2\n"
        "Format: 0x10002\n"
        "Sections: 1\n"
        "Seekable: yes\n"
        "\rDone: 0%"
        "\rDone: 7%"
        "\rDone: 14%"
        "\rDone: 21%"
        "\rDone: 28%"
        "\rDone: 35%"
        "\rDone: 42%"
        "\rDone: 50%"
        "\rDone: 57%"
        "\rDone: 64%"
        "\rDone: 71%"
        "\rDone: 78%"
        "\rDone: 85%"
        "\rDone: 92%"
        "\rDone: 100%\n"
        "Read 114624 frames\n"
    );

    ASSERT_THAT(output.str(), StrEq(expected_output));
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldProcessMonoWavFile)
{
    bool result = reader_.open("../test/data/test_file_mono.wav");
    ASSERT_TRUE(result);

    StrictMock<MockAudioProcessor> processor;

    InSequence sequence; // Calls expected in the order listed below.

    EXPECT_CALL(processor, init(16000, 1, 16384)).WillOnce(Return(true));

    // Total number of frames: 114624, 6 x 16384 frames then 1 x 16320
    EXPECT_CALL(processor, process(_, 16384)).Times(6).WillRepeatedly(Return(true));
    EXPECT_CALL(processor, process(_, 16320)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(processor, done());

    result = reader_.run(processor);

    std::string expected_output(
        "Input file: ../test/data/test_file_mono.wav\n"
        "Frames: 114624\n"
        "Sample rate: 16000 Hz\n"
        "Channels: 1\n"
        "Format: 0x10002\n"
        "Sections: 1\n"
        "Seekable: yes\n"
        "\rDone: 0%"
        "\rDone: 14%"
        "\rDone: 28%"
        "\rDone: 42%"
        "\rDone: 57%"
        "\rDone: 71%"
        "\rDone: 85%"
        "\rDone: 100%\n"
        "Read 114624 frames\n"
    );

    ASSERT_THAT(output.str(), StrEq(expected_output));
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldNotProcessFileMoreThanOnce)
{
    bool result = reader_.open("../test/data/test_file_stereo.wav");
    ASSERT_TRUE(result);

    StrictMock<MockAudioProcessor> processor;

    InSequence sequence; // Calls expected in the order listed below.

    EXPECT_CALL(processor, init(16000, 2, 16384)).WillOnce(Return(true));

    // Total number of frames: 114624, 13 x 8192 frames then 1 x 8128
    EXPECT_CALL(processor, process(_, 8192)).Times(13).WillRepeatedly(Return(true));
    EXPECT_CALL(processor, process(_, 8128)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(processor, done()).Times(1);

    result = reader_.run(processor);

    // Attempting to process the file again should fail
    output.str(std::string());

    result = reader_.run(processor);
    ASSERT_FALSE(result);

    ASSERT_TRUE(output.str().empty());
    ASSERT_TRUE(error.str().empty());
}

//------------------------------------------------------------------------------

TEST_F(SndFileAudioFileReaderTest, shouldReportErrorIfNotAWavFile)
{
    const char* filename = "../test/data/test_file_stereo.mp3";

    bool result = reader_.open(filename);

    ASSERT_FALSE(result);

    std::string str = error.str();
    ASSERT_THAT(str, HasSubstr(filename));
    ASSERT_THAT(str, EndsWith("\n"));
}

//------------------------------------------------------------------------------
