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

#include "WaveformColors.h"
#include "Rgba.h"

//------------------------------------------------------------------------------

WaveformColors::WaveformColors()
{
}

//------------------------------------------------------------------------------

WaveformColors::WaveformColors(
    const RGBA& border,
    const RGBA& background,
    const RGBA& waveform,
    const RGBA& axis_label) :
    border_color(border),
    background_color(background),
    waveform_color(waveform),
    axis_label_color(axis_label)
{
}

//------------------------------------------------------------------------------

bool WaveformColors::hasAlpha() const
{
    return border_color.hasAlpha() ||
           background_color.hasAlpha() ||
           waveform_color.hasAlpha() ||
           axis_label_color.hasAlpha();
}

//------------------------------------------------------------------------------

const WaveformColors audacity_waveform_colors(
    {0, 0, 0},
    {214, 214, 214},
    {63, 77, 155},
    {0, 0, 0}
);

//------------------------------------------------------------------------------

const WaveformColors audition_waveform_colors(
    {157, 157, 157},
    {0, 63, 34},
    {134, 252, 199},
    {190, 190, 190}
);

//------------------------------------------------------------------------------
