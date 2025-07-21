//------------------------------------------------------------------------------
//
// Copyright 2013-2024 BBC Research and Development
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

#include "Options.h"
#include "Config.h"
#include "Error.h"
#include "FileFormat.h"
#include "Log.h"
#include "MathUtil.h"
#include "Streams.h"
#include "Rgba.h"
#include "AudioFileReader.h"

#include <iostream>
#include <limits>
#include <string>
#include <utility>

//------------------------------------------------------------------------------

namespace po = boost::program_options;

//------------------------------------------------------------------------------

Options::Options() :
    desc_("Options"),
    quiet_(false),
    help_(false),
    version_(false),
    split_channels_(false),
    has_input_format_(false),
    input_format_(FileFormat::FileFormat::Unknown),
    has_output_format_(false),
    output_format_(FileFormat::FileFormat::Unknown),
    start_time_(0.0),
    end_time_(0.0),
    has_end_time_(false),
    samples_per_pixel_(0),
    auto_samples_per_pixel_(false),
    has_samples_per_pixel_(false),
    pixels_per_second_(0),
    has_pixels_per_second_(false),
    image_width_(0),
    image_height_(0),
    bits_(16),
    has_bits_(false),
    bar_width_(1),
    bar_gap_(0),
    render_axis_labels_(true),
    auto_amplitude_scale_(false),
    amplitude_scale_(1.0),
    png_compression_level_(-1), // default
    raw_sample_rate_(0),
    raw_channels_(0)
{
}

//------------------------------------------------------------------------------

static bool hasOptionValue(const po::variables_map& variables_map, const char* option_name)
{
    const auto& option = variables_map[option_name];
    return !option.empty();
}

//------------------------------------------------------------------------------

static bool parseWaveformColors(
    const std::string& waveform_color,
    std::vector<RGBA>& waveform_colors)
{
    std::istringstream stream(waveform_color);
    std::string value;

    while (std::getline(stream, value, ',')) {
        RGBA color;

        if (color.parse(value)) {
            waveform_colors.push_back(color);
        }
        else {
            return false;
        }
    }

    return true;
}

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

bool Options::parseCommandLine(int argc, const char* const* argv)
{
    program_name_ = argv[0];

    std::string input_filename;
    std::string output_filename;

    std::string input_format;
    std::string output_format;

    std::string amplitude_scale;
    std::string samples_per_pixel;
    std::string waveform_color;

    desc_.add_options()(
        "help",
        "show help message"
    )(
        "version,v",
        "show version information"
    )(
        "quiet,q",
        "disable progress and information messages"
    )(
        "input-filename,i",
        po::value<std::string>(&input_filename),
        FileFormat::isSupported(FileFormat::Opus) ?
            "input file name (.mp3, .wav, .flac, .ogg, .oga, .opus, .dat, .json)" :
            "input file name (.mp3, .wav, .flac, .ogg, .oga, .dat, .json)"
    )(
        "output-filename,o",
        po::value<std::string>(&output_filename),
        "output file name (.wav, .dat, .png, .json)"
    )(
        "split-channels",
        "output multi-channel waveform data or image files"
    )(
        "input-format",
        po::value<std::string>(&input_format),
        FileFormat::isSupported(FileFormat::Opus) ?
            "input file format (mp3, wav, flac, ogg, raw, opus, dat, json)" :
            "input file format (mp3, wav, flac, ogg, raw, dat, json)"
    )(
        "output-format",
        po::value<std::string>(&output_format),
        "output file format (wav, dat, png, json)"
    )(
        "zoom,z",
        po::value<std::string>(&samples_per_pixel)->default_value("256"),
        "zoom level (samples per pixel)"
    )(
        "pixels-per-second",
        po::value<int>(&pixels_per_second_)->default_value(100),
        "zoom level (pixels per second)"
    )(
        "bits,b",
        po::value<int>(&bits_)->default_value(16),
        "bits (8 or 16)"
    )(
        "start,s",
        po::value<double>(&start_time_)->default_value(0.0),
        "start time (seconds)"
    )(
        "end,e",
        po::value<double>(&end_time_)->default_value(0.0),
        "end time (seconds)"
    )(
        "width,w",
        po::value<int>(&image_width_)->default_value(800),
        "image width (pixels)"
    )(
        "height,h",
        po::value<int>(&image_height_)->default_value(250),
        "image height (pixels)"
    )(
        "colors,c",
        po::value<std::string>(&color_scheme_)->default_value("audacity"),
        "color scheme (audition or audacity)"
    )(
        "border-color",
        po::value<RGBA>(&border_color_),
        "border color (rrggbb[aa])"
    )(
        "background-color",
        po::value<RGBA>(&background_color_),
        "background color (rrggbb[aa])"
    )(
        "waveform-color",
        po::value<std::string>(&waveform_color),
        "waveform color (rrggbb[aa])"
    )(
        "waveform-style",
        po::value<std::string>(&waveform_style_)->default_value("normal"),
        "waveform style (normal or bars)"
    )(
        "bar-width",
        po::value<int>(&bar_width_)->default_value(8),
        "bar width (pixels)"
    )(
        "bar-gap",
        po::value<int>(&bar_gap_)->default_value(4),
        "bar gap (pixels)"
    )(
        "bar-style",
        po::value<std::string>(&bar_style_)->default_value("square"),
        "bar style (square or rounded)"
    )(
        "axis-label-color",
        po::value<RGBA>(&axis_label_color_),
        "axis label color (rrggbb[aa])"
    )(
        "no-axis-labels",
        "render waveform image without axis labels"
    )(
        "with-axis-labels",
        "render waveform image with axis labels (default)"
    )(
        "amplitude-scale",
        po::value<std::string>(&amplitude_scale)->default_value("1.0"),
        "amplitude scale"
    )(
        "compression",
        po::value<int>(&png_compression_level_)->default_value(-1),
        "PNG compression level: 0 (none) to 9 (best), or -1 (default)"
    )(
        "raw-samplerate",
        po::value<int>(&raw_sample_rate_),
        "sample rate for raw audio input (Hz)"
    )(
        "raw-channels",
        po::value<int>(&raw_channels_),
        "number of channels for raw audio input"
    )(
        "raw-format",
        po::value<std::string>(&raw_format_),
        "format for raw audio input "
        "(s8, u8, s16le, s16be, s24le, s24be, s32le, s32be, "
        "f32le, f32be, f64le, f64be)"
    );

    po::variables_map variables_map;

    try {
        po::command_line_parser parser(argc, argv);
        parser.options(desc_);
        po::store(parser.run(), variables_map);

        help_    = variables_map.count("help") != 0;
        version_ = variables_map.count("version") != 0;

        if (help_ || version_) {
            return true;
        }

        quiet_ = variables_map.count("quiet") != 0;

        split_channels_ = variables_map.count("split-channels") != 0;

        render_axis_labels_ = variables_map.count("no-axis-labels") == 0;

        has_end_time_ = !variables_map["end"].defaulted();

        has_samples_per_pixel_ = !variables_map["zoom"].defaulted();
        has_pixels_per_second_ = !variables_map["pixels-per-second"].defaulted();

        has_bits_ = !variables_map["bits"].defaulted();

        po::notify(variables_map);

        has_border_color_     = hasOptionValue(variables_map, "border-color");
        has_background_color_ = hasOptionValue(variables_map, "background-color");
        has_waveform_color_   = hasOptionValue(variables_map, "waveform-color");
        has_axis_label_color_ = hasOptionValue(variables_map, "axis-label-color");

        if (has_waveform_color_) {
            if (!parseWaveformColors(waveform_color, waveform_colors_)) {
                reportError("Invalid waveform-color");
                return false;
            }
        }

        // TODO: po::value<boost::filesystem::path> doesn't allow filenames
        // containing spaces.
        // See https://github.com/boostorg/program_options/issues/69
        input_filename_ = input_filename;
        output_filename_ = output_filename;

        has_input_format_  = hasOptionValue(variables_map, "input-format");
        has_output_format_ = hasOptionValue(variables_map, "output-format");

        bool has_raw_sample_rate = hasOptionValue(variables_map, "raw-samplerate");
        bool has_raw_channels    = hasOptionValue(variables_map, "raw-channels");
        bool has_raw_format      = hasOptionValue(variables_map, "raw-format");

        if (input_filename_.empty() && !has_input_format_) {
            reportError("Must specify either input filename or input format");
            return false;
        }

        input_format_ = has_input_format_ ?
            FileFormat::fromString(input_format) :
            getFormatFromFileExtension(input_filename_);
        FileFormat::getFormatViaSndfile(input_filename);

        handleAmplitudeScaleOption(amplitude_scale);
        handleZoomOption(samples_per_pixel);

        if (output_filename_.empty() && !has_output_format_) {
            reportError("Must specify either output filename or output format");
            return false;
        }

        output_format_ = has_output_format_ ?
            FileFormat::fromString(output_format) :
            getFormatFromFileExtension(output_filename_);

        if (bits_ != 8 && bits_ != 16) {
            reportError("Invalid bits: must be either 8 or 16");
            return false;
        }

        if (png_compression_level_ < -1 || png_compression_level_ > 9) {
            reportError("Invalid compression level: must be from 0 (none) to 9 (best), or -1 (default)");
            return false;
        }

        if (input_format_ == FileFormat::Raw) {
            if (!has_raw_sample_rate) {
                reportError("Missing --raw-samplerate option");
                return false;
            }

            if (!has_raw_channels) {
                reportError("Missing --raw-channels option");
                return false;
            }

            if (!has_raw_format) {
                reportError("Missing --raw-format option");
                return false;
            }
        }
    }
    catch (const std::runtime_error& e) {
        reportError(e.what());
        return false;
    }
    catch (const po::error& e) {
        reportError(e.what());
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

void Options::handleAmplitudeScaleOption(const std::string& option_value)
{
    if (option_value == "auto") {
        auto_amplitude_scale_ = true;
    }
    else {
        std::pair<bool, double> result = MathUtil::parseNumber(option_value);

        if (result.first) {
            if (result.second >= 0.0) {
                amplitude_scale_ = result.second;
            }
            else {
                throwError("Invalid amplitude scale: must be a positive number");
            }
        }
        else {
            throwError("Invalid amplitude scale: must be a number");
        }
    }
}

//------------------------------------------------------------------------------

void Options::handleZoomOption(const std::string& option_value)
{
    if (option_value == "auto") {
        auto_samples_per_pixel_ = true;
    }
    else {
        try {
            samples_per_pixel_ = std::stoi(option_value);
        }
        catch (std::invalid_argument& e) {
            throwError("Invalid zoom: must be a number or 'auto'");
        }
        catch (std::out_of_range& e) {
            throwError("Invalid zoom: number too large");
        }
    }
}

//------------------------------------------------------------------------------

void Options::showUsage(std::ostream& stream) const
{
    showVersion(stream);

    stream << "\nUsage:\n"
           << "  " << program_name_ << " [options]\n\n"
           << desc_
           << "\nSee audiowaveform(1) for usage examples\n";
}

//------------------------------------------------------------------------------

void Options::showVersion(std::ostream& stream) const
{
    stream << "AudioWaveform v"
           << VERSION_MAJOR << '.'
           << VERSION_MINOR << '.'
           << VERSION_PATCH << '\n';
}

//------------------------------------------------------------------------------

void Options::reportError(const std::string& message) const
{
    log(Error) << "Error: " << message
               << "\nSee '" << program_name_
               << " --help' for available options\n";
}

//------------------------------------------------------------------------------
