//------------------------------------------------------------------------------
//
// Copyright 2013 BBC Research and Development
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

#if !defined(INC_WAVEFORM_RESCALER_H)
#define INC_WAVEFORM_RESCALER_H

//------------------------------------------------------------------------------

class WaveformBuffer;

//------------------------------------------------------------------------------

class WaveformRescaler
{
    public:
        WaveformRescaler();

        WaveformRescaler(const WaveformRescaler&) = delete;
        WaveformRescaler& operator=(const WaveformRescaler&) = delete;

    public:
        bool rescale(
            const WaveformBuffer& input_buffer,
            WaveformBuffer& output_buffer,
            int samples_per_pixel
        );

    private:
        int sampleAtPixel(int x) const;

    private:
        int sample_rate_;
        int output_samples_per_pixel_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_WAVEFORM_RESCALER_H)

//------------------------------------------------------------------------------
