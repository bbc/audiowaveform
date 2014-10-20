//------------------------------------------------------------------------------
//
// Copyright 2014 BBC Research and Development
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

#include "Rgba.h"

#include "gmock/gmock.h"

#include <stdexcept>

//------------------------------------------------------------------------------

using testing::Eq;

//------------------------------------------------------------------------------

TEST(RgbaTest, shouldParseColorWithDigits)
{
    std::istringstream stream("112233");

    RGBA color;
    stream >> color;

    ASSERT_THAT(color.red,   Eq(0x11));
    ASSERT_THAT(color.green, Eq(0x22));
    ASSERT_THAT(color.blue,  Eq(0x33));
    ASSERT_THAT(color.alpha, Eq(0xFF));
}

//------------------------------------------------------------------------------

TEST(RgbaTest, shouldParseColorWithTransparency)
{
    std::istringstream stream("11223344");

    RGBA color;
    stream >> color;

    ASSERT_THAT(color.red,   Eq(0x11));
    ASSERT_THAT(color.green, Eq(0x22));
    ASSERT_THAT(color.blue,  Eq(0x33));
    ASSERT_THAT(color.alpha, Eq(0x44));
}

//------------------------------------------------------------------------------

TEST(RgbaTest, shouldParseColorWithLowerCaseHex)
{
    std::istringstream stream("aabbccdd");

    RGBA color;
    stream >> color;

    ASSERT_THAT(color.red,   Eq(0xAA));
    ASSERT_THAT(color.green, Eq(0xBB));
    ASSERT_THAT(color.blue,  Eq(0xCC));
    ASSERT_THAT(color.alpha, Eq(0xDD));
}

//------------------------------------------------------------------------------

TEST(RgbaTest, shouldParseColorWithUpperCaseHex)
{
    std::istringstream stream("AABBCCDD");

    RGBA color;
    stream >> color;

    ASSERT_THAT(color.red,   Eq(0xAA));
    ASSERT_THAT(color.green, Eq(0xBB));
    ASSERT_THAT(color.blue,  Eq(0xCC));
    ASSERT_THAT(color.alpha, Eq(0xDD));
}

//------------------------------------------------------------------------------

TEST(RgbaTest, shouldThrowIfEmptyString)
{
    std::istringstream stream("");

    RGBA color;
    ASSERT_THROW(stream >> color, std::runtime_error);
}

//------------------------------------------------------------------------------

TEST(RgbaTest, shouldThrowIfNotAValidColor)
{
    std::istringstream stream("1122XX");

    RGBA color;
    ASSERT_THROW(stream >> color, std::runtime_error);
}

//------------------------------------------------------------------------------
