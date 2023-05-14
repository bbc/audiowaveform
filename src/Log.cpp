//------------------------------------------------------------------------------
//
// Copyright 2021 BBC Research and Development
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

#include "Log.h"
#include "Streams.h"

#include <iostream>

//------------------------------------------------------------------------------

class NullStreamBuf : public std::streambuf
{
    public:
        virtual int overflow(int ch)
        {
            return ch;
        }
};

static NullStreamBuf null_streambuf;

static std::ostream null_stream(&null_streambuf);

//------------------------------------------------------------------------------

static bool quiet_ = false;

//------------------------------------------------------------------------------

void setLogLevel(bool quiet)
{
    quiet_ = quiet;
}

//------------------------------------------------------------------------------

std::ostream& log(LogLevel level)
{
    switch (level) {
        case Error:
            return error_stream;

        case Info:
        default:
            return quiet_ ? null_stream : error_stream;
    }
}

//------------------------------------------------------------------------------
