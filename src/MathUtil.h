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

#if !defined(INC_MATH_UTIL_H)
#define INC_MATH_UTIL_H

//------------------------------------------------------------------------------

#include <string>
#include <utility>

//------------------------------------------------------------------------------

namespace MathUtil {
    int roundDownToNearest(double value, int multiple);
    int roundUpToNearest(double value, int multiple);
    std::pair<bool, double> parseNumber(const std::string& value);
}

//------------------------------------------------------------------------------

#endif // #if !defined(INC_MATH_UTIL_H)

//------------------------------------------------------------------------------
