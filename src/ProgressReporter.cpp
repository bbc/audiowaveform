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
#include "Streams.h"

#include <sys/stat.h>

#include <iomanip>
#include <iostream>

//------------------------------------------------------------------------------

ProgressReporter::ProgressReporter() :
    show_progress_(true),
    percent_(-1) // Force first update to display 0%
{
    // If we're reading from a pipe, we may not know what the total duration is,
    // so don't report progress in this case.

    struct stat stat_buf;

    int result = fstat(fileno(stdin), &stat_buf);

    if (result >= 0) {
        if (S_ISFIFO(stat_buf.st_mode)) {
            show_progress_ = false;
        }
    }
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

        error_stream << "\rDone: " << percent << "%" << std::flush;
    }
}

//------------------------------------------------------------------------------
