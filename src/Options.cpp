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

#include "Options.h"
#include "Config.h"
#include "Streams.h"
#include "Rgba.h"

#include <iostream>

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
    image_width_(0),
    image_height_(0),
    render_axis_labels_(true)
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
        "border color (rrggbbaa)"
    )(
        "background-color",
        po::value<RGBA>(&background_color_),
        "background color (rrggbbaa)"
    )(
        "waveform-color",
        po::value<RGBA>(&waveform_color_),
        "wave color (rrggbbaa)"
    )(
        "axis-label-color",
        po::value<RGBA>(&axis_label_color_),
        "axis label color (rrggbbaa)"
    )(
        "no-axis-labels",
        "render waveform image without axis labels"
    )(
        "with-axis-labels",
        "render waveform image with axis labels (default)"
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

        po::notify(variables_map);

        has_border_color_     = hasOptionValue(variables_map, "border-color");
        has_background_color_ = hasOptionValue(variables_map, "background-color");
        has_waveform_color_   = hasOptionValue(variables_map, "waveform-color");
        has_axis_label_color_ = hasOptionValue(variables_map, "axis-label-color");

        if (bits_ != 8 && bits_ != 16) {
            error_stream << "Invalid bits: must be either 8 or 16\n";
            success = false;
        }
    }
    catch (std::runtime_error& e) {
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

void Options::showUsage(std::ostream& stream)
{
    showVersion(stream);

    stream << "\nUsage:\n"
           << "  " << program_name_ << " [options]\n\n"
           << desc_
           << "\nExamples:\n"
           << "  Generate waveform data from an MP3 file, at 256 samples per point\n"
           << "  with 8-bit resolution:\n"
           << "    " << program_name_ << " -i test.mp3 -o test.dat -z 256 -b 8\n\n"

           << "  Generate a 1000x200 pixel PNG image from a waveform data file\n"
           << "  at 512 samples per pixel, starting at 5.0 seconds:\n"
           << "    " << program_name_ << " -i test.dat -o test.png -z 512 -s 5.0 -w 1000 -h 200\n\n"

           << "  Generate a 1000x2000 pixel PNG image from a waveform data file\n"
           << "  starting at 5.0 seconds, ending at 10.0 seconds:\n"
           << "    " << program_name_ << " -i test.dat -o test.png -s 5.0 -e 10.0 -w 1000 -h 200\n\n"

           << "  Convert a waveform data file to JSON format:\n"
           << "    " << program_name_ << " -i test.dat -o test.json\n\n"

           << "  Convert MP3 to WAV format audio:\n"
           << "    " << program_name_ << " -i test.mp3 -o test.wav\n";
}

//------------------------------------------------------------------------------

void Options::showVersion(std::ostream& stream)
{
    stream << "AudioWaveform v"
           << VERSION_MAJOR << '.'
           << VERSION_MINOR << '.'
           << VERSION_PATCH << '\n';
}

//------------------------------------------------------------------------------

void Options::reportError(const std::exception& e)
{
    error_stream << "Error: " << e.what()
             << "\nSee '" << program_name_
             << " --help' for available options\n";
}

//------------------------------------------------------------------------------
