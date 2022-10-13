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
#include "FileUtil.h"
#include "Log.h"

#include <iomanip>
#include <iostream>

//------------------------------------------------------------------------------

// If we're reading from a pipe, we may not know what the total duration is,
// so don't report progress in this case.

ProgressReporter::ProgressReporter() :
    show_progress_(!FileUtil::isStdinFifo()),
    percent_(-1) // Force first update to display 0%
{
}

//------------------------------------------------------------------------------

void ProgressReporter::update(long long done, long long total)
{
    if (!show_progress_) {
        return;
    }

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

//------------------------------------------------------------------------------
