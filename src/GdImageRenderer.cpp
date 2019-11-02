//------------------------------------------------------------------------------
//
// Copyright 2013-2019 BBC Research and Development
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
#include "FileUtil.h"
#include "MathUtil.h"
#include "Streams.h"
#include "TimeUtil.h"
#include "WaveformBuffer.h"
#include "WaveformColors.h"

#include <gdfonts.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>

//------------------------------------------------------------------------------

// Upper limits prevent numeric overflows in image rendering.

const int MAX_SAMPLE_RATE   = 50000;
const int MAX_ZOOM          = 2000000;
const double MAX_START_TIME = 12 * 60 * 60; // 12 hours

//------------------------------------------------------------------------------

// Returns the minimum and maximum values over a given region of the buffer.

static std::pair<int, int> getAmplitudeRange(
    const WaveformBuffer& buffer,
    int start_index,
    int end_index)
{
    int low  = std::numeric_limits<int>::max();
    int high = std::numeric_limits<int>::min();

    const int channels = buffer.getChannels();

    for (int i = start_index; i != end_index; ++i) {
        for (int channel = 0; channel < channels; ++channel) {
            const int min = buffer.getMinSample(channel, i);
            const int max = buffer.getMaxSample(channel, i);

            if (min < low) {
                low = min;
            }

            if (max > high) {
                high = max;
            }
        }
    }

    return std::make_pair(low, high);
}

//------------------------------------------------------------------------------

// Multiply value by amplitude_scale, but limit the output to -32768 to 32767.

static int scale(int value, double amplitude_scale)
{
    double result = value * amplitude_scale;

    if (result > 32767.0) {
        result = 32767.0;
    }
    else if (result < -32768.0) {
        result = -32768.0;
    }

    return static_cast<int>(result);
}

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

bool GdImageRenderer::create(
    const WaveformBuffer& buffer,
    const double start_time,
    const int image_width,
    const int image_height,
    const WaveformColors& colors,
    const bool render_axis_labels,
    const bool auto_amplitude_scale,
    const double amplitude_scale)
{
    if (start_time < 0.0) {
        error_stream << "Invalid start time: minimum 0\n";
        return false;
    }
    else if (start_time > MAX_START_TIME) {
        error_stream << "Invalid start time: maximum " << MAX_START_TIME << '\n';
        return false;
    }

    if (image_width < 1) {
        error_stream << "Invalid image width: minimum 1\n";
        return false;
    }

    if (image_height < 1) {
        error_stream << "Invalid image height: minimum 1\n";
        return false;
    }

    const int sample_rate = buffer.getSampleRate();

    if (sample_rate <= 0) {
        error_stream << "Invalid sample rate: " << sample_rate << " Hz\n";
        return false;
    }

    if (sample_rate > MAX_SAMPLE_RATE) {
        error_stream << "Invalid sample rate: " << sample_rate
                     << " Hz, maximum " << MAX_SAMPLE_RATE << " Hz\n";
        return false;
    }

    if (buffer.getSize() < 1) {
        error_stream << "Empty waveform buffer\n";
        return false;
    }

    const int samples_per_pixel = buffer.getSamplesPerPixel();

    if (samples_per_pixel < 1) {
        error_stream << "Invalid waveform scale: " << samples_per_pixel << "\n";
        return false;
    }

    if (samples_per_pixel > MAX_ZOOM) {
        error_stream << "Invalid zoom: maximum " << MAX_ZOOM << '\n';
        return false;
    }

    image_ = gdImageCreateTrueColor(image_width, image_height);

    if (image_ == nullptr) {
        error_stream << "Failed to create image\n";
        return false;
    }

    assert(sample_rate != 0);
    assert(samples_per_pixel != 0);

    image_width_          = image_width;
    image_height_         = image_height;
    start_time_           = start_time;
    sample_rate_          = buffer.getSampleRate();
    samples_per_pixel_    = samples_per_pixel;
    start_index_          = secondsToPixels(start_time);
    render_axis_labels_   = render_axis_labels;
    auto_amplitude_scale_ = auto_amplitude_scale;
    amplitude_scale_      = amplitude_scale;
    channels_             = buffer.getChannels();

    error_stream << "Image dimensions: " << image_width_ << "x" << image_height_ << " pixels"
                 << "\nChannels: " << channels_
                 << "\nSample rate: " << sample_rate_ << " Hz"
                 << "\nSamples per pixel: " << samples_per_pixel_
                 << "\nStart time: " << start_time_ << " seconds"
                 << "\nStart index: " << start_index_
                 << "\nBuffer size: " << buffer.getSize()
                 << "\nAxis labels: " << (render_axis_labels_ ? "yes" : "no")
                 << "\n";

    if (colors.hasAlpha()) {
        gdImageSaveAlpha(image_, 1);
        gdImageAlphaBlending(image_, 0);
    }

    initColors(colors);
    drawBackground();

    if (render_axis_labels_) {
        drawBorder();
    }

    drawWaveform(buffer);

    if (render_axis_labels_) {
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
    // Avoid drawing over the right border
    const int max_x = render_axis_labels_ ? image_width_ - 1 : image_width_;

    // Avoid drawing over the top and bottom borders
    const int top_y = render_axis_labels_ ? 1 : 0;
    const int bottom_y = render_axis_labels_ ? image_height_ - 2 : image_height_ - 1;

    const int buffer_size = buffer.getSize();

    // Avoid drawing over the left border
    const int start_x     = render_axis_labels_ ? 1 : 0;
    const int start_index = render_axis_labels_ ? start_index_ + 1 : start_index_;

    double amplitude_scale;

    if (auto_amplitude_scale_) {
        int end_index = start_index + max_x;

        if (end_index > buffer_size) {
            end_index = buffer_size;
        }

        std::pair<int, int> range = getAmplitudeRange(buffer, start_index, end_index);

        double amplitude_scale_high = (range.second == 0) ? 1.0 : 32767.0 / range.second;
        double amplitude_scale_low  = (range.first  == 0) ? 1.0 : 32767.0 / range.first;

        amplitude_scale = std::fabs(std::min(amplitude_scale_high, amplitude_scale_low));
    }
    else {
        amplitude_scale = amplitude_scale_;
    }

    error_stream << "Amplitude scale: " << amplitude_scale << '\n';

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

        for (int i = start_index, x = start_x; x < max_x && i < buffer_size; ++i, ++x) {
            // Convert range [-32768, 32727] to [0, 65535]
            int low  = scale(buffer.getMinSample(channel, i), amplitude_scale) + 32768;
            int high = scale(buffer.getMaxSample(channel, i), amplitude_scale) + 32768;

            // Scale to fit the bitmap
            int high_y = waveform_top_y + height - 1 - high * height / 65536;
            int low_y  = waveform_top_y + height - 1 - low  * height / 65536;

            gdImageLine(image_, x, low_y, x, high_y, waveform_color_);
        }

        available_height -= row_height + 1;
        waveform_top_y += row_height + 1;
    }
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
        const int x = axis_label_offset_pixels +
            (secs - first_axis_label_secs) * sample_rate_ / samples_per_pixel_;

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
            error_stream << "Failed to write PNG file: "
                         << filename << '\n'
                         << strerror(errno) << '\n';
            return false;
        }
    }

    error_stream << "Output file: "
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
