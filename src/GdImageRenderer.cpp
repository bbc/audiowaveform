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

#include "GdImageRenderer.h"
#include "Array.h"
#include "Error.h"
#include "FileUtil.h"
#include "Log.h"
#include "MathUtil.h"
#include "TimeUtil.h"
#include "WaveformBuffer.h"
#include "WaveformColors.h"
#include "WaveformUtil.h"

#include <gdfonts.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>

//------------------------------------------------------------------------------

GdImageRenderer::GdImageRenderer() :
    image_(nullptr),
    image_width_(0),
    image_height_(0),
    start_time_(0.0),
    channels_(0),
    sample_rate_(0),
    samples_per_pixel_(0),
    start_index_(0),
    border_color_(0),
    background_color_(0),
    waveform_color_(0),
    axis_label_color_(0),
    waveform_style_bars_(false),
    bar_width_(8),
    bar_gap_(4),
    bar_style_rounded_(false),
    render_axis_labels_(true),
    auto_amplitude_scale_(false),
    amplitude_scale_(1.0)
{
}

//------------------------------------------------------------------------------

GdImageRenderer::~GdImageRenderer()
{
    if (image_ != nullptr) {
        gdImageDestroy(image_);
        image_ = nullptr;
    }
}

//------------------------------------------------------------------------------

bool GdImageRenderer::setStartTime(double start_time)
{
    if (start_time < 0.0) {
        log(Error) << "Invalid start time: minimum 0\n";
        return false;
    }

    start_time_ = start_time;

    return true;
}

//------------------------------------------------------------------------------

bool GdImageRenderer::setBarStyle(
    int bar_width,
    int bar_gap,
    bool bar_style_rounded)
{
    waveform_style_bars_ = true;

    if (bar_width < 1) {
        log(Error) << "Invalid bar width: minimum 1\n";
        return false;
    }

    if (bar_gap < 1) {
        log(Error) << "Invalid bar gap: minimum 1\n";
        return false;
    }

    bar_width_ = bar_width;
    bar_gap_ = bar_gap;
    bar_style_rounded_ = bar_style_rounded;

    return true;
}

//------------------------------------------------------------------------------

void GdImageRenderer::setAmplitudeScale(
    bool auto_amplitude_scale,
    double amplitude_scale)
{
    auto_amplitude_scale_ = auto_amplitude_scale;
    amplitude_scale_ = amplitude_scale;
}

//------------------------------------------------------------------------------

void GdImageRenderer::enableAxisLabels(bool render_axis_labels)
{
    render_axis_labels_ = render_axis_labels;
}

//------------------------------------------------------------------------------

bool GdImageRenderer::create(
    const WaveformBuffer& buffer,
    const int image_width,
    const int image_height,
    const WaveformColors& colors)
{
    if (image_width < 1) {
        log(Error) << "Invalid image width: minimum 1\n";
        return false;
    }

    if (image_height < 1) {
        log(Error) << "Invalid image height: minimum 1\n";
        return false;
    }

    const int sample_rate = buffer.getSampleRate();

    if (sample_rate <= 0) {
        log(Error) << "Invalid sample rate: " << sample_rate << " Hz\n";
        return false;
    }

    if (buffer.getSize() < 1) {
        log(Error) << "Empty waveform buffer\n";
        return false;
    }

    const int samples_per_pixel = buffer.getSamplesPerPixel();

    if (samples_per_pixel < 1) {
        log(Error) << "Invalid waveform scale: " << samples_per_pixel << "\n";
        return false;
    }

    image_ = gdImageCreateTrueColor(image_width, image_height);

    if (image_ == nullptr) {
        log(Error) << "Failed to create image\n";
        return false;
    }

    assert(sample_rate != 0);
    assert(samples_per_pixel != 0);

    image_width_       = image_width;
    image_height_      = image_height;
    sample_rate_       = buffer.getSampleRate();
    samples_per_pixel_ = samples_per_pixel;
    start_index_       = secondsToPixels(start_time_);
    channels_          = buffer.getChannels();

    log(Info) << "Image dimensions: " << image_width_ << "x" << image_height_ << " pixels"
              << "\nChannels: " << channels_
              << "\nSample rate: " << sample_rate_ << " Hz"
              << "\nSamples per pixel: " << samples_per_pixel_
              << "\nStart time: " << start_time_ << " seconds"
              << "\nStart index: " << start_index_
              << "\nBuffer size: " << buffer.getSize()
              << "\nAxis labels: " << (render_axis_labels_ ? "yes" : "no")
              << "\nWaveform style: " << (waveform_style_bars_ ? "bars" : "normal")
              << '\n';

    if (waveform_style_bars_) {
        log(Info) << "Bar width: " << bar_width_
                  << "\nBar gap: " << bar_gap_
                  << "\nBar style: " << (bar_style_rounded_ ? "rounded" : "square")
                  << '\n';
    }

    if (colors.hasAlpha()) {
        gdImageSaveAlpha(image_, 1);
        gdImageAlphaBlending(image_, 0);
    }

    initColors(colors);

    drawBackground();

    if (waveform_style_bars_) {
        drawWaveformBars(buffer);
    }
    else {
        drawWaveform(buffer);
    }

    if (render_axis_labels_) {
        drawBorder();
        drawTimeAxisLabels();
    }

    return true;
}

//------------------------------------------------------------------------------

int GdImageRenderer::createColor(const RGBA& color)
{
    if (color.hasAlpha()) {
        return gdImageColorAllocateAlpha(image_, color.red, color.green, color.blue, 127 - (color.alpha / 2));
    }
    else {
        return gdImageColorAllocate(image_, color.red, color.green, color.blue);
    }
}

//------------------------------------------------------------------------------

void GdImageRenderer::initColors(const WaveformColors& colors)
{
    border_color_     = createColor(colors.border_color);
    background_color_ = createColor(colors.background_color);
    waveform_color_   = createColor(colors.waveform_color);
    axis_label_color_ = createColor(colors.axis_label_color);
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawBackground() const
{
    gdImageFilledRectangle(image_, 0, 0, image_width_ - 1, image_height_ - 1, background_color_);
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawBorder() const
{
    gdImageRectangle(image_, 0, 0, image_width_ - 1, image_height_ - 1, border_color_);
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawWaveform(const WaveformBuffer& buffer) const
{
    // Avoid drawing over the top and bottom borders
    const int top_y   = render_axis_labels_ ? 1 : 0;
    const int bottom_y = render_axis_labels_ ? image_height_ - 2 : image_height_ - 1;

    const int buffer_size = buffer.getSize();

    double amplitude_scale;

    if (auto_amplitude_scale_) {
        int end_index = start_index_ + image_width_;

        if (end_index > buffer_size) {
            end_index = buffer_size;
        }

        amplitude_scale = WaveformUtil::getAmplitudeScale(buffer, start_index_, end_index);
    }
    else {
        amplitude_scale = amplitude_scale_;
    }

    log(Info) << "Amplitude scale: " << amplitude_scale << '\n';

    const int channels = buffer.getChannels();

    int available_height = bottom_y - top_y + 1;

    const int row_height = available_height / channels;

    int waveform_top_y = render_axis_labels_ ? 1 : 0;

    for (int channel = 0; channel < channels; ++channel) {
        int waveform_bottom_y;

        if (channel == channels - 1) {
            waveform_bottom_y = waveform_top_y + available_height - 1;
        }
        else {
            waveform_bottom_y = waveform_top_y + row_height;
        }

        const int height = waveform_bottom_y - waveform_top_y + 1;

        for (int i = start_index_, x = 0; x < image_width_ && i < buffer_size; ++i, ++x) {
            // Convert range [-32768, 32727] to [0, 65535]
            int low  = MathUtil::scale(buffer.getMinSample(channel, i), amplitude_scale) + 32768;
            int high = MathUtil::scale(buffer.getMaxSample(channel, i), amplitude_scale) + 32768;

            // Scale to fit the bitmap
            int top    = waveform_top_y + height - 1 - high * height / 65536;
            int bottom = waveform_top_y + height - 1 - low  * height / 65536;

            drawLine(x, top, x, bottom);
        }

        available_height -= row_height + 1;
        waveform_top_y += row_height + 1;
    }
}

//------------------------------------------------------------------------------

static int getBarHeight(const WaveformBuffer& buffer, int channel, int start, int width)
{
    int low = std::numeric_limits<int>::max();
    int high = std::numeric_limits<int>::min();

    int index = start;
    const int size = buffer.getSize();

    if (start >= size) {
        return 0;
    }

    for (int i = 0; i < width && index < size; ++i, ++index) {
        int sample = buffer.getMinSample(channel, index);

        if (sample < low) {
            low = sample;
        }

        sample = buffer.getMaxSample(channel, index);

        if (sample > high) {
            high = sample;
        }
    }

    if (low < 0) {
        low = -low;
    }

    if (high < 0) {
        high = -high;
    }

    low  = MathUtil::clamp(low,  0, std::numeric_limits<short>::max());
    high = MathUtil::clamp(high, 0, std::numeric_limits<short>::max());

    return low > high ? low : high;
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawWaveformBars(const WaveformBuffer& buffer) const
{
    // Avoid drawing over the top and bottom borders
    const int top_y    = render_axis_labels_ ? 1 : 0;
    const int bottom_y = render_axis_labels_ ? image_height_ - 2 : image_height_ - 1;

    const int buffer_size = buffer.getSize();

    double amplitude_scale;

    if (auto_amplitude_scale_) {
        int end_index = start_index_ + image_width_;

        if (end_index > buffer_size) {
            end_index = buffer_size;
        }

        amplitude_scale = WaveformUtil::getAmplitudeScale(buffer, start_index_, end_index);
    }
    else {
        amplitude_scale = amplitude_scale_;
    }

    log(Info) << "Amplitude scale: " << amplitude_scale << '\n';

    const int channels = buffer.getChannels();

    int available_height = bottom_y - top_y + 1;

    const int row_height = available_height / channels;

    int waveform_top_y = render_axis_labels_ ? 1 : 0;

    const int bar_total = bar_width_ + bar_gap_;

    // Round start_index down to the nearest start of bar
    int bar_start_index = (start_index_ / bar_total) * bar_total;

    int bar_start_offset = bar_start_index - start_index_;

    for (int channel = 0; channel < channels; ++channel) {
        int waveform_bottom_y;

        if (channel == channels - 1) {
            waveform_bottom_y = waveform_top_y + available_height - 1;
        }
        else {
            waveform_bottom_y = waveform_top_y + row_height;
        }

        const int height = waveform_bottom_y - waveform_top_y + 1;

        for (int i = bar_start_index, x = bar_start_offset; x < image_width_; i += bar_total, x += bar_total) {
            int bar_height = getBarHeight(buffer, channel, i, bar_total);

            // Convert range [-32768, 32727] to [0, 65535]
            int low  = MathUtil::scale(-bar_height, amplitude_scale) + 32768;
            int high = MathUtil::scale(bar_height, amplitude_scale) + 32768;

            // Scale to fit the bitmap
            int top    = waveform_top_y + height - 1 - high * height / 65536;
            int bottom = waveform_top_y + height - 1 - low  * height / 65536;

            if (top != bottom) {
                if (bar_style_rounded_ && bar_width_ > 2) {
                    const int radius = bar_width_ > 4 ? static_cast<int>(bar_width_ / 4)
                                                      : static_cast<int>(bar_width_ / 2);

                    drawRoundedRectangle(x, top, x + bar_width_ - 1, bottom, radius);
                }
                else {
                    drawRectangle(x, top, x + bar_width_ - 1, bottom);
                }
            }
        }

        available_height -= row_height + 1;
        waveform_top_y += row_height + 1;
    }
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawRoundedRectangle(
    const int left,
    const int top,
    const int right,
    const int bottom,
    const int radius) const
 {
    const int left_arc_x = left + radius;
    const int top_arc_y = top + radius;
    const int right_arc_x = right - radius;
    const int bottom_arc_y = bottom - radius;

    if (bottom_arc_y > top_arc_y) {
        // Draw the vertical bar
        drawRectangle(left, top_arc_y, right, bottom_arc_y);

        // Draw the top-left corner
        drawArc(left_arc_x, top_arc_y, radius * 2, radius * 2, 180, 270);
        // Draw the top-right corner
        drawArc(right_arc_x, top_arc_y, radius * 2, radius * 2, 270, 0);

        // Fill between top-left corner and top-right corner
        drawRectangle(left_arc_x, top, right_arc_x, top_arc_y);

        // Draw the bottom-left corner
        drawArc(left_arc_x, bottom_arc_y, radius * 2, radius * 2, 90, 180);
        // Draw the bottom-right corner
        drawArc(right_arc_x, bottom_arc_y, radius * 2, radius * 2, 0, 90);

        // Fill between bottom-left corner and bottom-right corner
        drawRectangle(left_arc_x, bottom_arc_y, right_arc_x, bottom);
    }
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawRectangle(int left, int top, int right, int bottom) const
{
    gdImageFilledRectangle(image_, left, top, right, bottom, waveform_color_);
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawArc(
    int centre_x,
    int centre_y,
    int width,
    int height,
    int start,
    int end) const
{
    gdImageFilledArc(
        image_,
        centre_x,
        centre_y,
        width,
        height,
        start,
        end,
        waveform_color_,
        gdStyledBrushed
    );
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawLine(int x1, int y1, int x2, int y2) const
{
    gdImageLine(image_, x1, y1, x2, y2, waveform_color_);
}

//------------------------------------------------------------------------------

void GdImageRenderer::drawTimeAxisLabels() const
{
    const int marker_height = 10;

    // Time interval between axis markers (seconds)
    const int axis_label_interval_secs = getAxisLabelScale();

    // Time of first axis marker (seconds)
    const int first_axis_label_secs = MathUtil::roundUpToNearest(start_time_, axis_label_interval_secs);

    // Distance between waveform start time and first axis marker (seconds)
    const double axis_label_offset_secs = first_axis_label_secs - start_time_;

    // Distance between waveform start time and first axis marker (samples)
    const int axis_label_offset_samples = secondsToSamples(axis_label_offset_secs);

    // Distance between waveform start time and first axis marker (pixels)
    const int axis_label_offset_pixels = axis_label_offset_samples / samples_per_pixel_;

    assert(axis_label_offset_pixels >= 0);

    gdFontPtr font = gdFontGetSmall();

    int secs = first_axis_label_secs;

    for (;;) {
        const int x = static_cast<int>(
            axis_label_offset_pixels +
            (secs - first_axis_label_secs) * static_cast<long long>(sample_rate_) / samples_per_pixel_
        );

        assert(x >= 0);

        if (x >= image_width_) {
            break;
        }

        gdImageLine(image_, x, 0, x, marker_height, border_color_);
        gdImageLine(image_, x, image_height_ - 1, x, image_height_ - 1 - marker_height, border_color_);

        char label[50];
        const int label_length = TimeUtil::secondsToString(label, ARRAY_LENGTH(label), secs);

        const int label_width = font->w * label_length;
        const int label_x = x - (label_width / 2) + 1;
        const int label_y = image_height_ - 1 - marker_height - 1 - font->h;

        if (label_x >= 0) {
            gdImageString(
                image_,
                font,
                label_x,
                label_y,
                reinterpret_cast<unsigned char*>(label),
                axis_label_color_
            );
        }

        secs += axis_label_interval_secs;
    }
}

//------------------------------------------------------------------------------

// Returns number of seconds for each x-axis marker, appropriate for the
// current zoom level, ensuring that markers are not too close together and
// that markers are placed at intuitive time intervals (i.e., every 1, 2, 5, 10,
// 20, 30 seconds, then every 1, 2, 5, 10, 20, 30 minutes, then every 1, 2, 5,
// 10, 20, 30 hours).

int GdImageRenderer::getAxisLabelScale() const
{
    int base_secs = 1; // seconds

    const int steps[] = { 1, 2, 5, 10, 20, 30 };

    const int MIN_SPACING = 60; // pixels

    int index = 0;

    int secs;

    for (;;) {
        secs = base_secs * steps[index];

        int pixels = secondsToPixels(secs);

        if (pixels < MIN_SPACING) {
            if (++index == ARRAY_LENGTH(steps)) {
                base_secs *= 60; // seconds -> minutes -> hours
                index = 0;
            }
        }
        else {
            // Spacing OK
            break;
        }
    }

    return secs;
}

//------------------------------------------------------------------------------

bool GdImageRenderer::saveAsPng(
    const char* filename,
    const int compression_level) const
{
    FILE* output_file;

    if (FileUtil::isStdioFilename(filename)) {
        output_file = stdout;
    }
    else {
        output_file = fopen(filename, "wb");

        if (output_file == nullptr) {
            log(Error) << "Failed to write PNG file: "
                       << filename << '\n'
                       << strerror(errno) << '\n';

            return false;
        }
    }

    log(Info) << "Output file: "
              << FileUtil::getOutputFilename(filename) << '\n';

    gdImagePngEx(image_, output_file, compression_level);

    if (output_file != stdout) {
        fclose(output_file);
    }

    return true;
}

//------------------------------------------------------------------------------

int GdImageRenderer::secondsToPixels(const double seconds) const
{
    return static_cast<int>(seconds * sample_rate_ / samples_per_pixel_);
}

//------------------------------------------------------------------------------
