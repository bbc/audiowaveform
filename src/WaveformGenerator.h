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

#if !defined(INC_WAVEFORM_GENERATOR_H)
#define INC_WAVEFORM_GENERATOR_H

//------------------------------------------------------------------------------

#include "AudioProcessor.h"

//------------------------------------------------------------------------------

class WaveformBuffer;

//------------------------------------------------------------------------------

class ScaleFactor
{
    public:
        virtual ~ScaleFactor();

    public:
        virtual int getSamplesPerPixel(int sample_rate) const = 0;
};

//------------------------------------------------------------------------------

class SamplesPerPixelScaleFactor : public ScaleFactor
{
    public:
        SamplesPerPixelScaleFactor(int samples_per_pixel);

    public:
        virtual int getSamplesPerPixel(int sample_rate) const;

    private:
        int samples_per_pixel_;
};

//------------------------------------------------------------------------------

class PixelsPerSecondScaleFactor : public ScaleFactor
{
    public:
        PixelsPerSecondScaleFactor(int pixels_per_second);

    public:
        virtual int getSamplesPerPixel(int sample_rate) const;

    private:
        int pixels_per_second_;
};

//------------------------------------------------------------------------------

class DurationScaleFactor : public ScaleFactor
{
    public:
        DurationScaleFactor(
            double start_time,
            double end_time,
            int width_pixels
        );

    public:
        virtual int getSamplesPerPixel(int sample_rate) const;

    private:
        double start_time_;
        double end_time_;
        int width_pixels_;
};

//------------------------------------------------------------------------------

class WaveformGenerator : public AudioProcessor
{
    public:
        WaveformGenerator(
            WaveformBuffer& buffer,
            const ScaleFactor& scale_factor
        );

        WaveformGenerator(const WaveformGenerator&) = delete;
        WaveformGenerator& operator=(const WaveformGenerator&) = delete;

    public:
        virtual bool init(
            int sample_rate,
            int channels,
            int buffer_size
        );

        int getSamplesPerPixel() const;

        virtual bool process(
            const short* input_buffer,
            int input_frame_count
        );

        virtual void done();

    private:
        void reset();

    private:
        WaveformBuffer& buffer_;
        const ScaleFactor& scale_factor_;

        int channels_;
        int samples_per_pixel_;

        int count_;
        int min_;
        int max_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_WAVEFORM_GENERATOR_H)

//------------------------------------------------------------------------------
