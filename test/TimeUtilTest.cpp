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

#include "TimeUtil.h"
#include "Array.h"

#include "gmock/gmock.h"

//------------------------------------------------------------------------------

using testing::Eq;
using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

TEST(TimeUtilTest, shouldConvertSecondsToString)
{
    char str[50];
    int result = TimeUtil::secondsToString(str, ARRAY_LENGTH(str), 1);
    ASSERT_THAT(str, StrEq("00:01"));
    ASSERT_THAT(result, Eq(5));

    result = TimeUtil::secondsToString(str, ARRAY_LENGTH(str), 59);
    ASSERT_THAT(str, StrEq("00:59"));
    ASSERT_THAT(result, Eq(5));

    result = TimeUtil::secondsToString(str, ARRAY_LENGTH(str), 61);
    ASSERT_THAT(str, StrEq("01:01"));
    ASSERT_THAT(result, Eq(5));

    result = TimeUtil::secondsToString(str, ARRAY_LENGTH(str), 3599);
    ASSERT_THAT(str, StrEq("59:59"));
    ASSERT_THAT(result, Eq(5));

    result = TimeUtil::secondsToString(str, ARRAY_LENGTH(str), 3661);
    ASSERT_THAT(str, StrEq("01:01:01"));
    ASSERT_THAT(result, Eq(8));

    result = TimeUtil::secondsToString(str, ARRAY_LENGTH(str), 7 * 86400 + 3661);
    ASSERT_THAT(str, StrEq("169:01:01"));
    ASSERT_THAT(result, Eq(9));

    char short_str[3];
    result = TimeUtil::secondsToString(short_str, ARRAY_LENGTH(short_str), 121);
    ASSERT_THAT(short_str, StrEq("02"));
    ASSERT_THAT(result, Eq(5));
}

//------------------------------------------------------------------------------
