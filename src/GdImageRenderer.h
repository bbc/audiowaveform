//------------------------------------------------------------------------------
//
// Copyright 2013-2023 BBC Research and Development
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

#if !defined(INC_GD_IMAGE_RENDERER_H)
#define INC_GD_IMAGE_RENDERER_H

//------------------------------------------------------------------------------

#include <gd.h>

//------------------------------------------------------------------------------

class RGBA;
class WaveformBuffer;
class WaveformColors;

//------------------------------------------------------------------------------

class GdImageRenderer
{
    public:
        GdImageRenderer();
        ~GdImageRenderer();

        GdImageRenderer(const GdImageRenderer&) = delete;
        GdImageRenderer& operator=(const GdImageRenderer&) = delete;

    public:
        bool setStartTime(double start_time);

        bool setBarStyle(
            int bar_width,
            int bar_gap,
            bool bar_style_rounded
        );

        void enableAxisLabels(bool render_axis_labels);

        void setAmplitudeScale(
            bool auto_amplitude_scale,
            double amplitude_scale
        );

        bool create(
            const WaveformBuffer& buffer,
            int image_width,
            int image_height,
            const WaveformColors& colors
        );

        bool saveAsPng(
            const char* filename,
            int compression_level = -1
        ) const;

    private:
        int createColor(const RGBA& color);

        void initColors(const WaveformColors& colors);

        void drawBackground() const;
        void drawBorder() const;

        void drawWaveform(const WaveformBuffer& buffer) const;
        void drawWaveformBars(const WaveformBuffer& buffer) const;

        void drawRoundedRectangle(
            const int x1,
            const int y1,
            const int x2,
            const int y2,
            const int radius
        ) const;

        void drawRectangle(int left, int top, int right, int bottom) const;

        void drawArc(
            int centre_x,
            int centre_y,
            int width,
            int height,
            int start,
            int end
        ) const;

        void drawLine(int x1, int y1, int x2, int y2) const;

        void drawTimeAxisLabels() const;

        int getAxisLabelScale() const;

        template<typename T>
        int secondsToSamples(T seconds) const
        {
            return static_cast<int>(sample_rate_ * seconds);
        }

        int secondsToPixels(const double seconds) const;

    private:
        gdImagePtr image_;
        int image_width_;
        int image_height_;

        double start_time_;
        int channels_;
        int sample_rate_;
        int samples_per_pixel_;
        int start_index_;

        int border_color_;
        int background_color_;
        int waveform_color_;
        int axis_label_color_;

        bool waveform_style_bars_;
        int bar_width_;
        int bar_gap_;
        bool bar_style_rounded_;

        bool render_axis_labels_;

        bool auto_amplitude_scale_;
        double amplitude_scale_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_GD_IMAGE_RENDERER_H)

//------------------------------------------------------------------------------
