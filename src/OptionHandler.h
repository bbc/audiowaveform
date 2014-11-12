//------------------------------------------------------------------------------
//
// Copyright 2014 BBC Research and Development
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

#if !defined(INC_OPTION_HANDLER_H)
#define INC_OPTION_HANDLER_H

//------------------------------------------------------------------------------

#include <boost/filesystem.hpp>

//------------------------------------------------------------------------------

class Options;

//------------------------------------------------------------------------------

class OptionHandler
{
    public:
        OptionHandler();

        OptionHandler(const OptionHandler&) = delete;
        OptionHandler& operator=(const OptionHandler&) = delete;

    public:
        bool run(const Options& options);

    private:
        bool convertAudioFormat(
            const boost::filesystem::path& input_filename,
            const boost::filesystem::path& output_filename
        );

        bool generateWaveformData(
            const boost::filesystem::path& input_filename,
            const boost::filesystem::path& output_filename,
            const Options& options
        );

        bool convertWaveformData(
            const boost::filesystem::path& input_filename,
            const boost::filesystem::path& output_filename,
            const Options& options
        );

        bool renderWaveformImage(
            const boost::filesystem::path& input_filename,
            const boost::filesystem::path& output_filename,
            const Options& options
        );
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_OPTION_HANDLER_H)

//------------------------------------------------------------------------------
