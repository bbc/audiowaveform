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
#include "Rgb.h"

//------------------------------------------------------------------------------

WaveformColors::WaveformColors()
{
}

//------------------------------------------------------------------------------

WaveformColors::WaveformColors(
    const RGB& border,
    const RGB& background,
    const RGB& waveform,
    const RGB& axis_label) :
    border_color(border),
    background_color(background),
    waveform_color(waveform),
    axis_label_color(axis_label)
{
}

//------------------------------------------------------------------------------

const WaveformColors audacityWaveformColors(
    RGB(0, 0, 0),
    RGB(214, 214, 214),
    RGB(63, 77, 155),
    RGB(0, 0, 0)
);

//------------------------------------------------------------------------------

const WaveformColors auditionWaveformColors(
    {157, 157, 157},
    {0, 63, 34},
    {134, 252, 199},
    {190, 190, 190}
);

//------------------------------------------------------------------------------
