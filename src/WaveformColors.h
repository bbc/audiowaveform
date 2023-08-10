//------------------------------------------------------------------------------
//
// Copyright 2014-2023 BBC Research and Development
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

#include "Rgba.h"

#include <vector>

//------------------------------------------------------------------------------

class WaveformColors
{
    public:
        WaveformColors();
        WaveformColors(
            const RGBA& border_color,
            const RGBA& background_color,
            const std::vector<RGBA>& waveform_colors,
            const RGBA& axis_label_color
        );

        bool hasAlpha() const;

    public:
        RGBA border_color;
        RGBA background_color;
        std::vector<RGBA> waveform_colors;
        RGBA axis_label_color;
};

//------------------------------------------------------------------------------

extern const WaveformColors audacity_waveform_colors;
extern const WaveformColors audition_waveform_colors;

//------------------------------------------------------------------------------

#endif // #if !defined(INC_WAVEFORM_COLORS_H)

//------------------------------------------------------------------------------
