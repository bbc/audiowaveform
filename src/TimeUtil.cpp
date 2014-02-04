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

#include <cstdio>

//------------------------------------------------------------------------------

namespace TimeUtil {

//------------------------------------------------------------------------------

int secondsToString(char* str, size_t size, int seconds)
{
    const int hours = seconds / 3600;

    seconds -= hours * 3600;

    const int minutes = seconds / 60;

    seconds -= minutes * 60;

    if (hours > 0) {
        return snprintf(str, size, "%02d:%02d:%02d", hours, minutes, seconds);
    }
    else {
        return snprintf(str, size, "%02d:%02d", minutes, seconds);
    }
}

//------------------------------------------------------------------------------

} // namespace TimeUtil

//------------------------------------------------------------------------------
