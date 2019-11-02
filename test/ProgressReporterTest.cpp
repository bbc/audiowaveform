//------------------------------------------------------------------------------
//
// Copyright 2013-2019 BBC Research and Development
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

#include "ProgressReporter.h"
#include "util/Streams.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <sstream>

//------------------------------------------------------------------------------

using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

class ProgressReporterTest : public Test
{
    protected:
        virtual void SetUp()
        {
            error.str(std::string());
        }

        virtual void TearDown()
        {
        }

    protected:
        ProgressReporter progress_reporter_;
};

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldDisplayZeroPercentWhenFirstCalled)
{
    progress_reporter_.update(0, 100);

    ASSERT_THAT(error.str(), StrEq("\rDone: 0%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldUpdatePercentage)
{
    progress_reporter_.update(0, 100);
    progress_reporter_.update(50, 100);
    progress_reporter_.update(100, 100);

    ASSERT_THAT(error.str(), StrEq("\rDone: 0%\rDone: 50%\rDone: 100%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldNotUpdatePercentageIfUnchanged)
{
    progress_reporter_.update(0, 100);
    progress_reporter_.update(50, 100);
    progress_reporter_.update(50, 100);
    progress_reporter_.update(100, 100);

    ASSERT_TRUE(output.str().empty());
    ASSERT_THAT(error.str(), StrEq("\rDone: 0%\rDone: 50%\rDone: 100%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldAllowPercentageToDecrease)
{
    progress_reporter_.update(0, 100);
    progress_reporter_.update(50, 100);
    progress_reporter_.update(25, 100);

    ASSERT_THAT(error.str(), StrEq("\rDone: 0%\rDone: 50%\rDone: 25%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldLimitPercentageAt0)
{
    progress_reporter_.update(-100, 100);

    ASSERT_THAT(error.str(), StrEq("\rDone: 0%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldLimitPercentageAt100)
{
    progress_reporter_.update(200, 100);

    ASSERT_THAT(error.str(), StrEq("\rDone: 100%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldNotAssumeTotalIs100)
{
    progress_reporter_.update(0, 1000);
    progress_reporter_.update(50, 1000);
    progress_reporter_.update(100, 1000);

    ASSERT_THAT(error.str(), StrEq("\rDone: 0%\rDone: 5%\rDone: 10%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldDisplayPercentageAsWholeNumber)
{
    progress_reporter_.update(50, 101);

    ASSERT_THAT(error.str(), StrEq("\rDone: 49%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldDisplayZeroIfTotalIsZero)
{
    progress_reporter_.update(50, 0);

    ASSERT_THAT(error.str(), StrEq("\rDone: 0%"));
}

//------------------------------------------------------------------------------

TEST_F(ProgressReporterTest, shouldAllowLargeNumbers)
{
    progress_reporter_.update(5000000000LL, 10000000000LL);

    ASSERT_THAT(error.str(), StrEq("\rDone: 50%"));
}

//------------------------------------------------------------------------------
