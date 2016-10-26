//------------------------------------------------------------------------------
//
// Copyright 2013, 2014, 2015 BBC Research and Development
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

#include "OptionHandler.h"
#include "Config.h"

#include "GdImageRenderer.h"
#include "Mp3AudioFileReader.h"
#include "Options.h"
#include "SndFileAudioFileReader.h"
#include "Streams.h"
#include "WaveformBuffer.h"
#include "WaveformColors.h"
#include "WaveformGenerator.h"
#include "WaveformRescaler.h"
#include "WavFileWriter.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <cassert>
#include <string>

//------------------------------------------------------------------------------

static std::unique_ptr<AudioFileReader> createAudioFileReader(
    const boost::filesystem::path& filename)
{
    std::unique_ptr<AudioFileReader> reader;

    const boost::filesystem::path ext = filename.extension();

    if (ext == ".wav" || ext == ".flac") {
        reader.reset(new SndFileAudioFileReader);
    }
    else if (ext == ".mp3") {
        reader.reset(new Mp3AudioFileReader);
    }
    else {
        const std::string message = boost::str(
            boost::format("Unknown file type: %1%") % filename
        );

        throw std::runtime_error(message);
    }

    return reader;
}

//------------------------------------------------------------------------------

static std::unique_ptr<ScaleFactor> createScaleFactor(const Options& options)
{
    std::unique_ptr<ScaleFactor> scale_factor;

    if ((options.hasSamplesPerPixel() || options.hasPixelsPerSecond()) &&
        options.hasEndTime()) {
        throw std::runtime_error("Specify either end time or zoom level, but not both");
    }
    else if (options.hasSamplesPerPixel() && options.hasPixelsPerSecond()) {
        throw std::runtime_error("Specify either zoom or pixels per second, but not both");
    }
    else if (options.hasEndTime()) {
        scale_factor.reset(new DurationScaleFactor(
            options.getStartTime(),
            options.getEndTime(),
            options.getImageWidth()
        ));
    }
    else if (options.hasPixelsPerSecond()) {
        scale_factor.reset(
            new PixelsPerSecondScaleFactor(options.getPixelsPerSecond())
        );
    }
    else {
        scale_factor.reset(
            new SamplesPerPixelScaleFactor(options.getSamplesPerPixel())
        );
    }

    return scale_factor;
}

//------------------------------------------------------------------------------

OptionHandler::OptionHandler()
{
}

//------------------------------------------------------------------------------

bool OptionHandler::convertAudioFormat(
    const boost::filesystem::path& input_filename,
    const boost::filesystem::path& output_filename)
{
    Mp3AudioFileReader reader;

    if (!reader.open(input_filename.c_str())) {
        return false;
    }

    WavFileWriter writer(output_filename.c_str());

    return reader.run(writer);
}

//------------------------------------------------------------------------------

bool OptionHandler::generateWaveformData(
    const boost::filesystem::path& input_filename,
    const boost::filesystem::path& output_filename,
    const Options& options)
{
    const std::unique_ptr<ScaleFactor> scale_factor = createScaleFactor(options);

    const boost::filesystem::path output_file_ext = output_filename.extension();

    const std::unique_ptr<AudioFileReader> audio_file_reader =
        createAudioFileReader(input_filename);

    if (audio_file_reader == nullptr) {
        error_stream << "Unknown file type: " << input_filename << '\n';
        return false;
    }

    if (!audio_file_reader->open(input_filename.c_str())) {
        return false;
    }

    WaveformBuffer buffer;
    WaveformGenerator processor(buffer, *scale_factor);

    if (!audio_file_reader->run(processor)) {
        return false;
    }

    assert(output_file_ext == ".dat" || output_file_ext == ".json");

    const int bits = options.getBits();

    if (output_file_ext == ".dat") {
        return buffer.save(output_filename.c_str(), bits);
    }
    else {
        return buffer.saveAsJson(output_filename.c_str(), bits);
    }
}

//------------------------------------------------------------------------------

bool OptionHandler::convertWaveformData(
    const boost::filesystem::path& input_filename,
    const boost::filesystem::path& output_filename,
    const Options& options)
{
    WaveformBuffer buffer;

    if (!buffer.load(input_filename.c_str())) {
        return false;
    }

    const int bits = options.hasBits() ? options.getBits() : buffer.getBits();

    bool success = true;

    const boost::filesystem::path output_file_ext = output_filename.extension();

    if (output_file_ext == ".json") {
        success = buffer.saveAsJson(output_filename.c_str(), bits);
    }
    else if (output_file_ext == ".txt") {
        success = buffer.saveAsText(output_filename.c_str(), bits);
    }

    return success;
}

//------------------------------------------------------------------------------

static WaveformColors createWaveformColors(const Options& options)
{
    WaveformColors colors;

    const std::string& color_scheme = options.getColorScheme();

    if (color_scheme == "audacity") {
        colors = audacity_waveform_colors;
    }
    else if (color_scheme == "audition") {
        colors = audition_waveform_colors;
    }
    else {
        std::string message = boost::str(
            boost::format("Unknown color scheme: %1%") % color_scheme
        );

        throw std::runtime_error(message);
    }

    if (options.hasBorderColor()) {
        colors.border_color = options.getBorderColor();
    }

    if (options.hasBackgroundColor()) {
        colors.background_color = options.getBackgroundColor();
    }

    if (options.hasWaveformColor()) {
        colors.waveform_color = options.getWaveformColor();
    }

    if (options.hasAxisLabelColor()) {
        colors.axis_label_color = options.getAxisLabelColor();
    }

    return colors;
}

//------------------------------------------------------------------------------

bool OptionHandler::renderWaveformImage(
    const boost::filesystem::path& input_filename,
    const boost::filesystem::path& output_filename,
    const Options& options)
{
    const std::unique_ptr<ScaleFactor> scale_factor = createScaleFactor(options);

    int output_samples_per_pixel = 0;

    WaveformBuffer input_buffer;

    const boost::filesystem::path input_file_ext = input_filename.extension();

    if (input_file_ext == ".dat") {
        if (!input_buffer.load(input_filename.c_str())) {
            return false;
        }

        output_samples_per_pixel = scale_factor->getSamplesPerPixel(
            input_buffer.getSampleRate()
        );
    }
    else {
        std::unique_ptr<AudioFileReader> audio_file_reader(
            createAudioFileReader(input_filename)
        );

        if (!audio_file_reader->open(input_filename.c_str())) {
            return false;
        }

        WaveformGenerator processor(input_buffer, *scale_factor);

        if (!audio_file_reader->run(processor)) {
            return false;
        }

        output_samples_per_pixel = input_buffer.getSamplesPerPixel();
    }

    WaveformBuffer output_buffer;
    WaveformBuffer* render_buffer = nullptr;

    const int input_samples_per_pixel = input_buffer.getSamplesPerPixel();

    if (output_samples_per_pixel == input_samples_per_pixel) {
        // No need to rescale
        render_buffer = &input_buffer;
    }
    else if (output_samples_per_pixel > input_samples_per_pixel) {
        WaveformRescaler rescaler;

        if (!rescaler.rescale(
            input_buffer,
            output_buffer,
            output_samples_per_pixel))
        {
            return false;
        }

        render_buffer = &output_buffer;
    }
    else {
        error_stream << "Invalid zoom, minimum: " << input_samples_per_pixel << '\n';
        return false;
    }

    const WaveformColors colors = createWaveformColors(options);

    GdImageRenderer renderer;

    if (!renderer.create(
        *render_buffer,
        options.getStartTime(),
        options.getImageWidth(),
        options.getImageHeight(),
        colors,
        options.getRenderAxisLabels(),
        options.isAutoAmplitudeScale(),
        options.getAmplitudeScale()))
    {
        return false;
    }

    return renderer.saveAsPng(
        output_filename.c_str(),
        options.getPngCompressionLevel()
    );
}

//------------------------------------------------------------------------------

bool OptionHandler::run(const Options& options)
{
    if (options.getHelp()) {
        options.showUsage(output_stream);
        return true;
    }
    else if (options.getVersion()) {
        options.showVersion(output_stream);
        return true;
    }

    const boost::filesystem::path input_filename  = options.getInputFilename();
    const boost::filesystem::path output_filename = options.getOutputFilename();

    const boost::filesystem::path input_file_ext  = input_filename.extension();
    const boost::filesystem::path output_file_ext = output_filename.extension();

    bool success;

    try {
        if (input_file_ext == ".mp3" && output_file_ext == ".wav") {
            success = convertAudioFormat(
                input_filename,
                output_filename
            );
        }
        else if ((input_file_ext == ".mp3" ||
                  input_file_ext == ".wav" ||
                  input_file_ext == ".flac") &&
                 (output_file_ext == ".dat" || output_file_ext == ".json")) {
            success = generateWaveformData(
                input_filename,
                output_filename,
                options
            );
        }
        else if (input_file_ext == ".dat" &&
                 (output_file_ext == ".txt" || output_file_ext == ".json")) {
            success = convertWaveformData(
                input_filename,
                output_filename,
                options
            );
        }
        else if ((input_file_ext == ".dat" ||
                  input_file_ext == ".mp3" ||
                  input_file_ext == ".wav" ||
                  input_file_ext == ".flac") && output_file_ext == ".png") {
            success = renderWaveformImage(
                input_filename,
                output_filename,
                options
            );
        }
        else {
            error_stream << "Can't generate " << output_filename
                         << " from " << input_filename << '\n';
            success = false;
        }
    }
    catch (const std::runtime_error& error) {
        error_stream << error.what() << "\n";
        success = false;
    }

    return success;
}

//------------------------------------------------------------------------------
