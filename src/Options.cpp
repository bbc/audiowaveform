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

#include "Options.h"
#include "Config.h"
#include "Streams.h"

#include <iostream>

//------------------------------------------------------------------------------

namespace po = boost::program_options;

//------------------------------------------------------------------------------

Options::Options() :
    desc_("Options"),
    help_(false),
    version_(false),
    start_time_(0.0),
    samples_per_pixel_(0),
    image_width_(0),
    image_height_(0)
{
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
        "input file name (.mp3, .wav, .dat)"
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
    );

    po::variables_map vm;

    try {
        po::command_line_parser parser(argc, argv);
        parser.options(desc_);
        po::store(parser.run(), vm);

        help_    = vm.count("help") != 0;
        version_ = vm.count("version") != 0;

        if (help_ || version_) {
            return true;
        }

        po::notify(vm);
    }
    catch (const po::error& e) {
        error_stream << "Error: " << e.what()
                     << "\nSee '" << program_name_
                     << " --help' for available options\n";

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

           << "  Convert a waveform data file to JSON format:\n"
           << "    " << program_name_ << " -i test.dat -o test.json\n\n"

           << "  Convert MP3 to WAV format audio:\n"
           << "    " << program_name_ << " -i test.mp3 -o test.wav\n";
}

//------------------------------------------------------------------------------

void Options::showVersion(std::ostream& stream)
{
    stream << "AudioWaveform v"
           << APP_VERSION_MAJOR << '.'
           << APP_VERSION_MINOR << '.'
           << APP_VERSION_PATCH << '\n';
}

//------------------------------------------------------------------------------
