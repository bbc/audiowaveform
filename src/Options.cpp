//------------------------------------------------------------------------------
//
// Copyright 2013-2016 BBC Research and Development
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
#include "MathUtil.h"
#include "Streams.h"
#include "Rgba.h"

#include <iostream>
#include <utility>

//------------------------------------------------------------------------------

namespace po = boost::program_options;

//------------------------------------------------------------------------------

Options::Options() :
    desc_("Options"),
    help_(false),
    version_(false),
    start_time_(0.0),
    end_time_(0.0),
    has_end_time_(false),
    samples_per_pixel_(0),
    has_samples_per_pixel_(false),
    pixels_per_second_(0),
    has_pixels_per_second_(false),
    image_width_(0),
    image_height_(0),
    bits_(16),
    has_bits_(false),
    render_axis_labels_(true),
    auto_amplitude_scale_(false),
    amplitude_scale_(1.0),
    png_compression_level_(-1) // default
{
}

//------------------------------------------------------------------------------

static bool hasOptionValue(const po::variables_map& variables_map, const char* option_name)
{
    const auto& option = variables_map[option_name];
    return !option.empty();
}

//------------------------------------------------------------------------------

bool Options::parseCommandLine(int argc, const char* const* argv)
{
    bool success = true;

    program_name_ = argv[0];

    std::string amplitude_scale;

    desc_.add_options()(
        "help",
        "show help message"
    )(
        "version,v",
        "show version information"
    )(
        "input-filename,i",
        po::value<std::string>(&input_filename_)->required(),
        "input file name (.mp3, .wav, .flac, .dat)"
    )(
        "output-filename,o",
        po::value<std::string>(&output_filename_)->required(),
        "output file name (.wav, .dat, .png, .json)"
    )(
        "zoom,z",
        po::value<int>(&samples_per_pixel_)->default_value(256),
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
        po::value<RGBA>(&waveform_color_),
        "wave color (rrggbb[aa])"
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

        render_axis_labels_ = variables_map.count("no-axis-labels") == 0;

        const auto& end_option = variables_map["end"];
        has_end_time_ = !end_option.defaulted();

        const auto& samples_per_pixel_option = variables_map["zoom"];
        has_samples_per_pixel_ = !samples_per_pixel_option.defaulted();

        const auto& pixels_per_second_option = variables_map["pixels-per-second"];
        has_pixels_per_second_ = !pixels_per_second_option.defaulted();

        const auto& bits_option = variables_map["bits"];
        has_bits_ = !bits_option.defaulted();

        po::notify(variables_map);

        has_border_color_     = hasOptionValue(variables_map, "border-color");
        has_background_color_ = hasOptionValue(variables_map, "background-color");
        has_waveform_color_   = hasOptionValue(variables_map, "waveform-color");
        has_axis_label_color_ = hasOptionValue(variables_map, "axis-label-color");

        if (bits_ != 8 && bits_ != 16) {
            error_stream << "Invalid bits: must be either 8 or 16\n";
            success = false;
        }

        handleAmpltideScaleOption(amplitude_scale);

        if (png_compression_level_ < -1 || png_compression_level_ > 9) {
            error_stream << "Invalid compression level: must be from 0 (none) to 9 (best), or -1 (default)\n";
            success = false;
        }
    }
    catch (const std::runtime_error& e) {
        reportError(e);
        success = false;
    }
    catch (const po::error& e) {
        reportError(e);
        success = false;
    }

    return success;
}

//------------------------------------------------------------------------------

void Options::handleAmpltideScaleOption(const std::string& option_value)
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

void Options::reportError(const std::exception& e) const
{
    error_stream << "Error: " << e.what()
                 << "\nSee '" << program_name_
                 << " --help' for available options\n";
}

//------------------------------------------------------------------------------
