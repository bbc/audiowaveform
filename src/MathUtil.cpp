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

#include "MathUtil.h"

#include <boost/regex.hpp>

#include <cassert>
#include <cmath>

//------------------------------------------------------------------------------

namespace MathUtil {

//------------------------------------------------------------------------------

// Rounds the given value down to the nearest given multiple.
// e.g: roundDownToNearest(5.5, 3) returns 3
//      roundDownToNearest(141.0, 10) returns 140
//      roundDownToNearest(-5.5, 3) returns -3

int roundDownToNearest(double value, int multiple)
{
    if (multiple == 0) {
        return 0;
    }

    return multiple * (static_cast<int>(value) / multiple);
}

//------------------------------------------------------------------------------

// Rounds the given value up to the nearest given multiple.
// e.g: roundUpToNearest(5.5, 3) returns 6
//      roundUpToNearest(141.0, 10) returns 150
//      roundUpToNearest(-5.5, 3) returns -6

int roundUpToNearest(double value, int multiple)
{
    if (multiple == 0) {
        return 0;
    }

    int multiplier = 1;

    if (value < 0.0) {
        multiplier = -1;
        value = -value;
    }

    const int rounded_up = static_cast<int>(ceil(value));

    return multiplier * ((rounded_up + multiple - 1) / multiple) * multiple;
}

//------------------------------------------------------------------------------

std::pair<bool, double> parseNumber(const std::string& value)
{
    static const boost::regex regex(
        "^"          // Anchor to beginning of string
        "[\\+\\-]?"  // Optionally, either a '+' or a '-'
        "\\d*"       // Zero or more digits
        "(\\.\\d*)?" // Optionally, a '.' followed by zero or more digits
        "$"          // Anchor to end of string
    );

    double number = 0.0;
    bool success;

    boost::smatch match;

    if (boost::regex_match(value, match, regex)) {
        try {
            number = std::stod(value);

            success = true;
        }
        catch (const std::exception& e) {
            success = false;
        }
    }
    else {
        success = false;
    }

    return std::make_pair(success, number);
}

//------------------------------------------------------------------------------

} // namespace MathUtil

//------------------------------------------------------------------------------
