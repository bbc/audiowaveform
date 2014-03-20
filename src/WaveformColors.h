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

#if !defined(INC_WAVEFORM_COLORS_H)
#define INC_WAVEFORM_COLORS_H

//------------------------------------------------------------------------------

#include "Rgb.h"

//------------------------------------------------------------------------------

class WaveformColors
{
    public:
        WaveformColors();
        WaveformColors(
            const RGB& border_color,
            const RGB& background_color,
            const RGB& wave_color,
            const RGB& axis_label_color
        );

    public:
        RGB border_color;
        RGB background_color;
        RGB waveform_color;
        RGB axis_label_color;
};

//------------------------------------------------------------------------------

extern const WaveformColors audacityWaveformColors;
extern const WaveformColors auditionWaveformColors;

//------------------------------------------------------------------------------

#endif // #if !defined(INC_WAVEFORM_COLORS_H)

//------------------------------------------------------------------------------
