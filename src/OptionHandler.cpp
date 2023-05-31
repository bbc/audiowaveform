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

#include "OptionHandler.h"
#include "Config.h"

#include "AudioLoader.h"
#include "DurationCalculator.h"
#include "Error.h"
#include "FileFormat.h"
#include "FileUtil.h"
#include "GdImageRenderer.h"
#include "Mp3AudioFileReader.h"
#include "Log.h"
#include "Options.h"
#include "SndFileAudioFileReader.h"
#include "Streams.h"
#include "VectorAudioFileReader.h"
#include "WaveformBuffer.h"
#include "WaveformColors.h"
#include "WaveformGenerator.h"
#include "WaveformRescaler.h"
#include "WaveformUtil.h"
#include "WavFileWriter.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <cassert>
#include <string>

//------------------------------------------------------------------------------

static std::string getFileExtension(const boost::filesystem::path& filename)
{
    std::string extension = filename.extension().string();

    // Remove leading "."
    if (!extension.empty()) {
        extension.erase(0, 1);
    }

    return extension;
}

//------------------------------------------------------------------------------

static FileFormat::FileFormat getFormatFromFileExtension(
    const boost::filesystem::path& filename)
{
    return FileFormat::fromString(getFileExtension(filename));
}

//------------------------------------------------------------------------------

static FileFormat::FileFormat getInputFormat(
    const Options& options,
    const boost::filesystem::path& filename)
{
    return options.hasInputFormat() ?
        FileFormat::fromString(options.getInputFormat()) :
        getFormatFromFileExtension(filename);
}

//------------------------------------------------------------------------------

static FileFormat::FileFormat getOutputFormat(
    const Options& options,
    const boost::filesystem::path& filename)
{
    return options.hasOutputFormat() ?
        FileFormat::fromString(options.getOutputFormat()) :
        getFormatFromFileExtension(filename);
}

//------------------------------------------------------------------------------

static std::unique_ptr<AudioFileReader> createAudioFileReader(
    const boost::filesystem::path& input_filename,
    const FileFormat::FileFormat input_format)
{
    std::unique_ptr<AudioFileReader> reader;

    if (input_format == FileFormat::Wav ||
        input_format == FileFormat::Flac ||
        input_format == FileFormat::Ogg ||
        input_format == FileFormat::Opus) {
        reader.reset(new SndFileAudioFileReader);
    }
    else if (input_format == FileFormat::Mp3) {
        reader.reset(new Mp3AudioFileReader);
    }
    else {
        throwError("Unknown file type: %1%", input_filename);
    }

    return reader;
}

//------------------------------------------------------------------------------

static std::unique_ptr<ScaleFactor> createScaleFactor(const Options& options)
{
    std::unique_ptr<ScaleFactor> scale_factor;

    if (options.hasSamplesPerPixel() && options.hasEndTime()) {
        throwError("Specify either --end or --zoom but not both");
    }
    else if (options.hasPixelsPerSecond() && options.hasEndTime()) {
        throwError("Specify either --end or --pixels-per-second but not both");
    }
    else if (options.hasSamplesPerPixel() && options.hasPixelsPerSecond()) {
        throwError("Specify either --zoom or --pixels-per-second but not both");
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

// Returns the equivalent audio duration of the given waveform buffer.

static double getDuration(const WaveformBuffer& buffer)
{
    return buffer.getSize() * buffer.getSamplesPerPixel() / buffer.getSampleRate();
}

//------------------------------------------------------------------------------

// Returns the duration of the given audio file, in seconds.

static std::pair<bool, double> getDuration(
    const boost::filesystem::path& input_filename,
    const FileFormat::FileFormat input_format,
    const bool verbose)
{
    std::unique_ptr<AudioFileReader> audio_file_reader(
        createAudioFileReader(input_filename, input_format)
    );

    if (!audio_file_reader->open(input_filename.string().c_str())) {
        return std::make_pair(false, 0);
    }

    if (verbose) {
        log(Info) << "Calculating audio duration...\n";
    }

    DurationCalculator duration_calculator;

    if (!audio_file_reader->run(duration_calculator)) {
        return std::make_pair(false, 0);
    }

    const double duration = duration_calculator.getDuration();

    if (verbose) {
        log(Info) << "Duration: " << duration << " seconds\n";
    }

    if (FileUtil::isStdioFilename(input_filename.string().c_str())) {
        if (fseek(stdin, 0, SEEK_SET) != 0) {
            log(Error) << "Failed to seek to start of audio\n";

            return std::make_pair(false, 0);
        }
    }

    return std::make_pair(true, duration);
}

//------------------------------------------------------------------------------

OptionHandler::OptionHandler()
{
}

//------------------------------------------------------------------------------

bool OptionHandler::convertAudioFormat(
    const boost::filesystem::path& input_filename,
    const FileFormat::FileFormat input_format,
    const boost::filesystem::path& output_filename)
{
    std::unique_ptr<AudioFileReader> reader(
        createAudioFileReader(input_filename, input_format)
    );

    if (!reader->open(input_filename.string().c_str())) {
        return false;
    }

    WavFileWriter writer(output_filename.string().c_str());

    return reader->run(writer);
}

//------------------------------------------------------------------------------

bool OptionHandler::generateWaveformData(
    const boost::filesystem::path& input_filename,
    const FileFormat::FileFormat input_format,
    const boost::filesystem::path& output_filename,
    const FileFormat::FileFormat output_format,
    const Options& options)
{
    const std::unique_ptr<ScaleFactor> scale_factor = createScaleFactor(options);

    const boost::filesystem::path output_file_ext = output_filename.extension();

    const std::unique_ptr<AudioFileReader> audio_file_reader =
        createAudioFileReader(input_filename, input_format);

    if (!audio_file_reader->open(input_filename.string().c_str())) {
        return false;
    }

    WaveformBuffer buffer;
    const bool split_channels = options.getSplitChannels();
    WaveformGenerator processor(buffer, split_channels, *scale_factor);

    if (!audio_file_reader->run(processor)) {
        return false;
    }

    if (options.isAutoAmplitudeScale() && buffer.getSize() > 0) {
        const double amplitude_scale = WaveformUtil::getAmplitudeScale(
            buffer, 0, buffer.getSize()
        );

        WaveformUtil::scaleWaveformAmplitude(buffer, amplitude_scale);
    }

    assert(output_format == FileFormat::Dat ||
           output_format == FileFormat::Json);

    const int bits = options.getBits();

    if (output_format == FileFormat::Dat) {
        return buffer.save(output_filename.string().c_str(), bits);
    }
    else {
        return buffer.saveAsJson(output_filename.string().c_str(), bits);
    }
}

//------------------------------------------------------------------------------

bool OptionHandler::convertWaveformData(
    const boost::filesystem::path& input_filename,
    const boost::filesystem::path& output_filename,
    const FileFormat::FileFormat output_format,
    const Options& options)
{
    WaveformBuffer buffer;

    if (!buffer.load(input_filename.string().c_str())) {
        return false;
    }

    const int bits = options.hasBits() ? options.getBits() : buffer.getBits();

    bool success = true;

    const boost::filesystem::path output_file_ext = output_filename.extension();

    if (output_format == FileFormat::Json) {
        success = buffer.saveAsJson(output_filename.string().c_str(), bits);
    }
    else if (output_format == FileFormat::Txt) {
        success = buffer.saveAsText(output_filename.string().c_str(), bits);
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
        throwError("Unknown color scheme: %1%", color_scheme);
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

static bool isWaveformStyleBars(const Options& options)
{
    const std::string& waveform_style = options.getWaveformStyle();

    bool waveform_style_bars = false;

    if (waveform_style == "bars") {
        waveform_style_bars = true;
    }
    else if (waveform_style != "normal") {
        throwError("Unknown waveform style: %1%", waveform_style);
    }

    return waveform_style_bars;
}

//------------------------------------------------------------------------------

static bool isBarStyleRounded(const Options& options)
{
    const std::string& bar_style = options.getBarStyle();

    bool bar_style_rounded = false;

    if (bar_style == "rounded") {
        bar_style_rounded = true;
    }
    else if (bar_style != "square") {
        throwError("Unknown waveform bar style: %1%", bar_style);
    }

    return bar_style_rounded;
}

//------------------------------------------------------------------------------

bool OptionHandler::renderWaveformImage(
    const boost::filesystem::path& input_filename,
    const FileFormat::FileFormat input_format,
    const boost::filesystem::path& output_filename,
    const Options& options)
{
    std::unique_ptr<ScaleFactor> scale_factor;

    const bool calculate_duration = options.isAutoSamplesPerPixel();

    if (!calculate_duration) {
        scale_factor = createScaleFactor(options);
    }

    const WaveformColors colors = createWaveformColors(options);

    GdImageRenderer renderer;

    if (!renderer.setStartTime(options.getStartTime())) {
        return false;
    }

    if (isWaveformStyleBars(options)) {
        if (!renderer.setBarStyle(
            options.getBarWidth(),
            options.getBarGap(),
            isBarStyleRounded(options)))
        {
            return false;
        }
    }

    renderer.setAmplitudeScale(
        options.isAutoAmplitudeScale(),
        options.getAmplitudeScale()
    );

    renderer.enableAxisLabels(options.getRenderAxisLabels());

    int output_samples_per_pixel = 0;

    WaveformBuffer input_buffer;

    if (input_format == FileFormat::Dat) {
        if (!input_buffer.load(input_filename.string().c_str())) {
            return false;
        }

        if (calculate_duration) {
            const double duration = getDuration(input_buffer);

            scale_factor.reset(
                new DurationScaleFactor(0.0, duration, options.getImageWidth())
            );
        }

        output_samples_per_pixel = scale_factor->getSamplesPerPixel(
            input_buffer.getSampleRate()
        );
    }
    else {
        // Seeking to the start of the audio won't work if reading from a pipe
        // or a socket, so we buffer the entire audio data in memory.

        if (FileUtil::isStdioFilename(input_filename.string().c_str()) &&
            !FileUtil::isStdinSeekable() &&
            calculate_duration) {

            std::unique_ptr<AudioFileReader> audio_file_reader(
                createAudioFileReader(input_filename, input_format)
            );

            if (!audio_file_reader->open(input_filename.string().c_str())) {
                return false;
            }

            AudioLoader loader;

            if (!audio_file_reader->run(loader)) {
                return false;
            }

            scale_factor.reset(
                new DurationScaleFactor(0.0, loader.getDuration(), options.getImageWidth())
            );

            const bool split_channels = options.getSplitChannels();

            WaveformGenerator processor(input_buffer, split_channels, *scale_factor);

            VectorAudioFileReader reader(loader.getData(), loader.getSampleRate(), loader.getChannels());

            if (!reader.run(processor)) {
                return false;
            }

            output_samples_per_pixel = input_buffer.getSamplesPerPixel();
        }
        else {
            if (calculate_duration) {
                auto result = getDuration(input_filename, input_format, !options.getQuiet());

                if (!result.first) {
                    return false;
                }

                double duration = result.second;

                scale_factor.reset(
                    new DurationScaleFactor(0.0, duration, options.getImageWidth())
                );
            }

            std::unique_ptr<AudioFileReader> audio_file_reader(
                createAudioFileReader(input_filename, input_format)
            );

            if (!audio_file_reader->open(input_filename.string().c_str(), !calculate_duration)) {
                return false;
            }

            const bool split_channels = options.getSplitChannels();

            WaveformGenerator processor(input_buffer, split_channels, *scale_factor);

            if (!audio_file_reader->run(processor)) {
                return false;
            }

            output_samples_per_pixel = input_buffer.getSamplesPerPixel();
        }
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

        rescaler.rescale(
            input_buffer,
            output_buffer,
            output_samples_per_pixel
        );

        render_buffer = &output_buffer;
    }
    else {
        log(Error) << "Invalid zoom, minimum: " << input_samples_per_pixel << '\n';
        return false;
    }

    if (!renderer.create(
        *render_buffer,
        options.getImageWidth(),
        options.getImageHeight(),
        colors))
    {
        return false;
    }

    return renderer.saveAsPng(
        output_filename.string().c_str(),
        options.getPngCompressionLevel()
    );
}

//------------------------------------------------------------------------------

bool OptionHandler::resampleWaveformData(
    const boost::filesystem::path& input_filename,
    const boost::filesystem::path& output_filename,
    const Options& options)
{
    WaveformBuffer input_buffer;

    if (!input_buffer.load(input_filename.string().c_str())) {
        return false;
    }

    std::unique_ptr<ScaleFactor> scale_factor = createScaleFactor(options);

    int output_samples_per_pixel = scale_factor->getSamplesPerPixel(
        input_buffer.getSampleRate()
    );

    WaveformBuffer output_buffer;
    WaveformRescaler rescaler;

    rescaler.rescale(
        input_buffer,
        output_buffer,
        output_samples_per_pixel
    );

    const int bits = options.getBits();

    return output_buffer.save(output_filename.string().c_str(), bits);
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

    setLogLevel(options.getQuiet());

    bool success = true;

    try {
        const boost::filesystem::path input_filename =
            options.getInputFilename();

        const boost::filesystem::path output_filename =
            options.getOutputFilename();

        const FileFormat::FileFormat input_format =
            getInputFormat(options, input_filename);

        const FileFormat::FileFormat output_format =
            getOutputFormat(options, output_filename);

        if ((input_format == FileFormat::Mp3 ||
             input_format == FileFormat::Flac ||
             input_format == FileFormat::Ogg ||
             input_format == FileFormat::Opus) &&
            FileFormat::isSupported(input_format) &&
            output_format == FileFormat::Wav) {
            success = convertAudioFormat(
                input_filename,
                input_format,
                output_filename
            );
        }
        else if ((input_format == FileFormat::Mp3 ||
                  input_format == FileFormat::Wav ||
                  input_format == FileFormat::Flac ||
                  input_format == FileFormat::Ogg ||
                  input_format == FileFormat::Opus) &&
                 FileFormat::isSupported(input_format) &&
                 (output_format == FileFormat::Dat ||
                  output_format == FileFormat::Json)) {
            success = generateWaveformData(
                input_filename,
                input_format,
                output_filename,
                output_format,
                options
            );
        }
        else if (input_format == FileFormat::Dat &&
                 (output_format == FileFormat::Txt ||
                  output_format == FileFormat::Json)) {
            success = convertWaveformData(
                input_filename,
                output_filename,
                output_format,
                options
            );
        }
        else if ((input_format == FileFormat::Dat ||
                  input_format == FileFormat::Mp3 ||
                  input_format == FileFormat::Wav ||
                  input_format == FileFormat::Flac ||
                  input_format == FileFormat::Ogg ||
                  input_format == FileFormat::Opus) &&
                 FileFormat::isSupported(input_format) &&
                 output_format == FileFormat::Png) {
            success = renderWaveformImage(
                input_filename,
                input_format,
                output_filename,
                options
            );
        }
        else if (input_format == FileFormat::Dat &&
                 output_format == FileFormat::Dat) {
            success = resampleWaveformData(
                input_filename,
                output_filename,
                options
            );
        }
        else {
            log(Error) << "Can't generate "
                       << FileFormat::toString(output_format)
                       << " format output from "
                       << FileFormat::toString(input_format)
                       << " format input\n";
            success = false;
        }
    }
    catch (const std::runtime_error& error) {
        log(Error) << error.what() << "\n";
        success = false;
    }

    if (success) {
        log(Info) << "Done\n";
    }

    return success;
}

//------------------------------------------------------------------------------
