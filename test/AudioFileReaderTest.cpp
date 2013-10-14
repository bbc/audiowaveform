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

#include "AudioFileReader.h"
#include "AudioProcessor.h"
#include "util/Streams.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <sstream>

//------------------------------------------------------------------------------

using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

class TestAudioFileReader : public AudioFileReader
{
    public:
        virtual bool open(const char* /* input_filename */)
        {
            return true;
        }

        virtual bool run(AudioProcessor& /* processor */)
        {
            return true;
        }

        void progress(long long done, long long total)
        {
            showProgress(done, total);
        }
};

//------------------------------------------------------------------------------

class AudioFileReaderTest : public Test
{
    protected:
        virtual void SetUp()
        {
            output.str(std::string());
        }

        virtual void TearDown()
        {
        }

        TestAudioFileReader reader_;
};

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldDisplayZeroPercentWhenFirstCalled)
{
    reader_.progress(0, 100);

    ASSERT_THAT(output.str(), StrEq("\rDone: 0%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldUpdatePercentage)
{
    reader_.progress(0, 100);
    reader_.progress(50, 100);
    reader_.progress(100, 100);

    ASSERT_THAT(output.str(), StrEq("\rDone: 0%\rDone: 50%\rDone: 100%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldNotUpdatePercentageIfUnchanged)
{
    reader_.progress(0, 100);
    reader_.progress(50, 100);
    reader_.progress(50, 100);
    reader_.progress(100, 100);

    ASSERT_THAT(output.str(), StrEq("\rDone: 0%\rDone: 50%\rDone: 100%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldAllowPercentageToDecrease)
{
    reader_.progress(0, 100);
    reader_.progress(50, 100);
    reader_.progress(25, 100);

    ASSERT_THAT(output.str(), StrEq("\rDone: 0%\rDone: 50%\rDone: 25%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldLimitPercentageAt0)
{
    reader_.progress(-100, 100);

    ASSERT_THAT(output.str(), StrEq("\rDone: 0%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldLimitPercentageAt100)
{
    reader_.progress(200, 100);

    ASSERT_THAT(output.str(), StrEq("\rDone: 100%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldNotAssumeTotalIs100)
{
    reader_.progress(0, 1000);
    reader_.progress(50, 1000);
    reader_.progress(100, 1000);

    ASSERT_THAT(output.str(), StrEq("\rDone: 0%\rDone: 5%\rDone: 10%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldDisplayPercentageAsWholeNumber)
{
    reader_.progress(50, 101);

    ASSERT_THAT(output.str(), StrEq("\rDone: 49%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldDisplayZeroIfTotalIsZero)
{
    reader_.progress(50, 0);

    ASSERT_THAT(output.str(), StrEq("\rDone: 0%"));
}

//------------------------------------------------------------------------------

TEST_F(AudioFileReaderTest, shouldAllowLargeNumbers)
{
    reader_.progress(5000000000LL, 10000000000LL);

    ASSERT_THAT(output.str(), StrEq("\rDone: 50%"));
}

//------------------------------------------------------------------------------
