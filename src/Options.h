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

#if !defined(INC_OPTIONS_H)
#define INC_OPTIONS_H

//------------------------------------------------------------------------------

#include "Rgba.h"

#include <boost/program_options.hpp>

#include <iosfwd>
#include <string>
#include <stdexcept>

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
        double getEndTime() const { return end_time_; }
        bool hasEndTime() const { return has_end_time_; }

        int getSamplesPerPixel() const { return samples_per_pixel_; }
        bool hasSamplesPerPixel() const { return has_samples_per_pixel_; }

        int getPixelsPerSecond() const { return pixels_per_second_; }
        bool hasPixelsPerSecond() const { return has_pixels_per_second_; }

        int getBits() const { return bits_; }
        bool hasBits() const { return has_bits_; }
        int getImageWidth() const { return image_width_; }
        int getImageHeight() const { return image_height_; }

        const std::string& getColorScheme() const { return color_scheme_; }

        const RGBA& getBorderColor() const { return border_color_; }
        const RGBA& getBackgroundColor() const { return background_color_; }
        const RGBA& getWaveformColor() const { return waveform_color_; }
        const RGBA& getAxisLabelColor() const { return axis_label_color_; }

        bool hasBorderColor() const { return has_border_color_; }
        bool hasBackgroundColor() const { return has_background_color_; }
        bool hasWaveformColor() const { return has_waveform_color_; }
        bool hasAxisLabelColor() const { return has_axis_label_color_; }

        bool getRenderAxisLabels() const { return render_axis_labels_; }

        bool isAutoAmplitudeScale() const { return auto_amplitude_scale_; }
        double getAmplitudeScale() const { return amplitude_scale_; }

        int getPngCompressionLevel() const { return png_compression_level_; }

        bool getHelp() const { return help_; }
        bool getVersion() const { return version_; }

        void showUsage(std::ostream& stream) const;
        void showVersion(std::ostream& stream) const;

        void reportError(const std::exception& e) const;

    private:
        void handleAmpltideScaleOption(const std::string& option_value);

    private:
        boost::program_options::options_description desc_;

        std::string program_name_;

        bool help_;
        bool version_;

        std::string input_filename_;
        std::string output_filename_;

        double start_time_;
        double end_time_;
        bool has_end_time_;

        int samples_per_pixel_;
        bool has_samples_per_pixel_;

        int pixels_per_second_;
        bool has_pixels_per_second_;

        int image_width_;
        int image_height_;
        int bits_;
        bool has_bits_;

        std::string color_scheme_;

        RGBA border_color_;
        RGBA background_color_;
        RGBA waveform_color_;
        RGBA axis_label_color_;

        bool has_border_color_;
        bool has_background_color_;
        bool has_waveform_color_;
        bool has_axis_label_color_;

        bool render_axis_labels_;

        bool auto_amplitude_scale_;
        double amplitude_scale_;

        int png_compression_level_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_OPTIONS_H)

//------------------------------------------------------------------------------
