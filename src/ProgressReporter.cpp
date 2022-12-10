//------------------------------------------------------------------------------
//
// Copyright 2013-2022 BBC Research and Development
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
#include "Array.h"
#include "FileUtil.h"
#include "Log.h"
#include "TimeUtil.h"

#include <iomanip>
#include <iostream>

//------------------------------------------------------------------------------

ProgressReporter::ProgressReporter() :
    percent_(-1), // Force first update to display 0%
    seconds_(-1)
{
}

//------------------------------------------------------------------------------

void ProgressReporter::update(double seconds, long long done, long long total)
{
    if (total != 0) {
        int percent;

        if (total > 0) {
            percent = static_cast<int>(done * 100 / total);

            if (percent < 0) {
                percent = 0;
            }
            else if (percent > 100) {
                percent = 100;
            }
        }
        else {
            percent = 0;
        }

        if (percent != percent_) {
            percent_ = percent;

            log(Info) << "\rDone: " << percent << "%" << std::flush;
        }
    }
    else {
        if (static_cast<int>(seconds) != seconds_) {
            seconds_ = static_cast<int>(seconds);

            char time[100];

            TimeUtil::secondsToString(time, ARRAY_LENGTH(time), seconds_);

            log(Info) << "\rDone: " << time << std::flush;
        }
    }
}

//------------------------------------------------------------------------------
