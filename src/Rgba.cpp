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

#include <boost/regex.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

//------------------------------------------------------------------------------

RGBA::RGBA() :
    red(0),
    green(0),
    blue(0),
    alpha(0)
{
}

//------------------------------------------------------------------------------

RGBA::RGBA(int r, int g, int b) :
    red(r),
    green(g),
    blue(b),
    alpha(255)
{
}

//------------------------------------------------------------------------------

RGBA::RGBA(int r, int g, int b, int a) :
    red(r),
    green(g),
    blue(b),
    alpha(a)
{
}

//------------------------------------------------------------------------------

std::istream& operator>>(std::istream& stream, RGBA& rgba)
{
    std::string value;
    stream >> value;

    static const boost::regex regex("^([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})$");

    boost::smatch match;

    if (boost::regex_match(value, match, regex)) {
        rgba.red   = static_cast<int>(strtol(match[1].str().c_str(), nullptr, 16));
        rgba.green = static_cast<int>(strtol(match[2].str().c_str(), nullptr, 16));
        rgba.blue  = static_cast<int>(strtol(match[3].str().c_str(), nullptr, 16));
        rgba.alpha = static_cast<int>(strtol(match[4].str().c_str(), nullptr, 16));
    }
    else {
        throw std::runtime_error("Invalid color value");
    }

    return stream;
}

//------------------------------------------------------------------------------
