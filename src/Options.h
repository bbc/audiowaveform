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

#if !defined(INC_OPTIONS_H)
#define INC_OPTIONS_H

//------------------------------------------------------------------------------

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iosfwd>
#include <string>

//------------------------------------------------------------------------------

class Options
{
    public:
        Options();

        Options(const Options&) = delete;
        Options& operator=(const Options&) = delete;

    public:
        bool parseCommandLine(int argc, const char* const* argv);

        const std::string& getInputFilename() const
        {
            return input_filename_;
        }

        const std::string& getOutputFilename() const
        {
            return output_filename_;
        }

        double getStartTime() const { return start_time_; }
        int getSamplesPerPixel() const { return samples_per_pixel_; }
        int getBits() const { return bits_; }
        int getImageWidth() const { return image_width_; }
        int getImageHeight() const { return image_height_; }

        const std::string& getColorScheme() const { return color_scheme_; }

        bool getHelp() const { return help_; }
        bool getVersion() const { return version_; }

        void showUsage(std::ostream& stream);
        void showVersion(std::ostream& stream);

    private:
        boost::program_options::options_description desc_;

        std::string program_name_;

        bool help_;
        bool version_;

        std::string input_filename_;
        std::string output_filename_;
        double start_time_;
        int samples_per_pixel_;
        int image_width_;
        int image_height_;
        int bits_;
        std::string color_scheme_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_OPTIONS_H)

//------------------------------------------------------------------------------
