//------------------------------------------------------------------------------
//
// Copyright 2019 BBC Research and Development
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

#include "FileFormat.h"

#include "Error.h"

#include <boost/algorithm/string.hpp>

#include <map>
#include <stdexcept>

//------------------------------------------------------------------------------

namespace FileFormat {

//------------------------------------------------------------------------------

FileFormat fromString(const std::string& name)
{
    const std::string key = boost::to_lower_copy(name);

    static const std::map<std::string, FileFormat> map{
        { "mp3",  FileFormat::Mp3  },
        { "wav",  FileFormat::Wav  },
        { "flac", FileFormat::Flac },
        { "ogg",  FileFormat::Ogg  },
        { "oga",  FileFormat::Ogg  },
        { "dat",  FileFormat::Dat  },
        { "json", FileFormat::Json },
        { "txt",  FileFormat::Txt  },
        { "png",  FileFormat::Png  }
    };

    const auto i = map.find(key);

    return i == map.end() ? FileFormat::Unknown : i->second;
}

//------------------------------------------------------------------------------

std::string getFileExt(FileFormat file_format)
{
    return "." + toString(file_format);
}

//------------------------------------------------------------------------------

std::string toString(FileFormat file_format)
{
    std::string str;

    switch (file_format) {
        case FileFormat::Mp3:
            str = "mp3";
            break;

        case FileFormat::Wav:
            str = "wav";
            break;

        case FileFormat::Flac:
            str = "flac";
            break;

        case FileFormat::Ogg:
            str = "ogg";
            break;

        case FileFormat::Dat:
            str = "dat";
            break;

        case FileFormat::Json:
            str = "json";
            break;

        case FileFormat::Txt:
            str = "txt";
            break;

        case FileFormat::Png:
            str = "png";
            break;

        default:
            throwError("Unknown file format");
            break;
    }

    return str;
}

//------------------------------------------------------------------------------

} // namespace FileFormat

//------------------------------------------------------------------------------
