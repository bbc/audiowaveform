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

#include "Config.h"
#include "Options.h"
#include "OptionHandler.h"

#include <iostream>
#include <limits>

//------------------------------------------------------------------------------

static_assert(std::numeric_limits<int>::max() >= 2147483647L, "size of int");

//------------------------------------------------------------------------------

std::ostream& output_stream = std::cout;
std::ostream& error_stream  = std::cerr;

//------------------------------------------------------------------------------

int main(int argc, const char* const* argv)
{
    Options options;

    if (!options.parseCommandLine(argc, argv)) {
        return 1;
    }

    OptionHandler option_handler;

    bool success = option_handler.run(options);

    return success ? 0 : 1;
}

//------------------------------------------------------------------------------
