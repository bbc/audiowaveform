//------------------------------------------------------------------------------
//
// Copyright 2024 BBC Research and Development
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
#include <sndfile.h>

#include <map>
#include <stdexcept>
#include <iostream>

//------------------------------------------------------------------------------

namespace FileFormat {

//------------------------------------------------------------------------------

FileFormat getFormatViaSndfile(const std::string& filePath)
{

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    SNDFILE* sf = sf_open(filePath.c_str(), SFM_READ, &sfinfo);

    if (!sf) {
        //TODO: Should mabye use the proper logging method here.
        std::cerr << "Error opening file: " << sf_strerror(NULL) << std::endl;
    }

    int majorFormat = sfinfo.format & SF_FORMAT_TYPEMASK;
    int minorFormat = sfinfo.format & SF_FORMAT_SUBMASK;
    
    //TODO  I might need to double check the performance of doing it this way 
    /*
        Also there is no check for png or anyother types of files 
        but I think it's good enough to just check the extension for 
        the other file types
    */

    sf_close(sf);
    
    static const std::map<int, FileFormat> audioFileFormatMap = {
        {SF_FORMAT_MPEG, FileFormat::Mp3}, 
        {SF_FORMAT_WAV,  FileFormat::Wav},
        {SF_FORMAT_W64,  FileFormat::Wav},
        {SF_FORMAT_WAVEX, FileFormat::Wav},
        {SF_FORMAT_RF64, FileFormat::Wav},
        {SF_FORMAT_FLAC, FileFormat::Flac}, 
        {SF_FORMAT_OGG,  FileFormat::Ogg}, 
        {SF_FORMAT_OPUS, FileFormat::Opus}
    };

    /* minor Edge case for opus as apparently opus doesn't have a major format SF_FORMAT_OPUS but matches in the minor_format
        https://libsndfile.github.io/libsndfile/api.html

        TODO: Double check if I need to check if I need to check for this within the typemask/top level format
        or if I can just leave it at checking the subformat and check.
        TODO: Check for any other edge cases
    */

    std::cout << "FORMAT>>>>>>>>" << majorFormat << std::endl;
    if (minorFormat == SF_FORMAT_OPUS) {
        return FileFormat::Opus;
    }

    auto it = audioFileFormatMap.find(majorFormat);

    return it == audioFileFormatMap.end() ? FileFormat::Unknown : it->second;
}


//------------------------------------------------------------------------------

FileFormat fromString(const std::string& name)
{
    const std::string key = boost::to_lower_copy(name);

    static const std::map<std::string, FileFormat> map{
        { "mp3",  FileFormat::Mp3  },
        { "wav",  FileFormat::Wav  },
        { "w64",  FileFormat::Wav  },
        { "flac", FileFormat::Flac },
        { "ogg",  FileFormat::Ogg  },
        { "oga",  FileFormat::Ogg  },
        { "opus", FileFormat::Opus },
        { "raw",  FileFormat::Raw  },
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

        case FileFormat::Opus:
            str = "opus";
            break;

        case FileFormat::Raw:
            str = "raw";
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

static bool isSndFileFormatSupported(int format)
{
    SF_INFO info;
    memset(&info, 0, sizeof(info));

    info.channels = 2;
    info.format = format;

    return sf_format_check(&info) != 0;
}

//------------------------------------------------------------------------------

bool isSupported(FileFormat file_format)
{
    if (file_format == FileFormat::Opus) {
        const int format = 0x200000 | 0x0064; // SF_FORMAT_OGG | SF_FORMAT_OPUS

        return isSndFileFormatSupported(format);
    }
    else {
        return true;
    }
}

//------------------------------------------------------------------------------

bool isAudioFormat(FileFormat file_format)
{
    return file_format == FileFormat::Mp3 ||
           file_format == FileFormat::Wav ||
           file_format == FileFormat::Flac ||
           file_format == FileFormat::Ogg ||
           file_format == FileFormat::Opus ||
           file_format == FileFormat::Raw;
}

//------------------------------------------------------------------------------

bool isWaveformDataFormat(FileFormat file_format)
{
    return file_format == FileFormat::Dat ||
           file_format == FileFormat::Json;
}

//------------------------------------------------------------------------------

} // namespace FileFormat

//------------------------------------------------------------------------------
