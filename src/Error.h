//------------------------------------------------------------------------------
//
// Copyright 2015 BBC Research and Development
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

#if !defined(INC_ERROR_H)
#define INC_ERROR_H

//------------------------------------------------------------------------------

#include <boost/format.hpp>

//------------------------------------------------------------------------------

void throwError(boost::format& message);

//------------------------------------------------------------------------------

template<typename Value, typename... Args>
void throwError(boost::format& message, const Value& value, Args... args)
{
    message % value;
    throwError(message, args...);
}

//------------------------------------------------------------------------------

template<typename... Args>
void throwError(const std::string& format, Args... args)
{
    boost::format message(format);
    throwError(message, args...);
}

//------------------------------------------------------------------------------

#endif // #if !defined(INC_ERROR_H)

//------------------------------------------------------------------------------
