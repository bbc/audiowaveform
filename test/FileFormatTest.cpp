//------------------------------------------------------------------------------
//
// Copyright 2020 BBC Research and Development
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

#include "FileFormat.h"

#include "gmock/gmock.h"

//------------------------------------------------------------------------------

using testing::Eq;
using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

TEST(FileFormatTest, shouldConvertFromString)
{
    ASSERT_THAT(FileFormat::fromString("wav"), Eq(FileFormat::Wav));
    ASSERT_THAT(FileFormat::fromString("WAV"), Eq(FileFormat::Wav));
    ASSERT_THAT(FileFormat::fromString("xyz"), Eq(FileFormat::Unknown));
}

//------------------------------------------------------------------------------

TEST(FileFormatTest, shouldConvertToString)
{
    ASSERT_THAT(FileFormat::toString(FileFormat::Wav), StrEq("wav"));
    ASSERT_THROW(FileFormat::toString(FileFormat::Unknown), std::runtime_error);
}

//------------------------------------------------------------------------------

TEST(FileFormatTest, shouldReturnFileExtension)
{
    ASSERT_THAT(FileFormat::getFileExt(FileFormat::Wav), StrEq(".wav"));
    ASSERT_THROW(FileFormat::getFileExt(FileFormat::Unknown), std::runtime_error);
}

//------------------------------------------------------------------------------
