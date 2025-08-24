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

#if !defined(INC_FILE_FORMAT_H)
#define INC_FILE_FORMAT_H

//------------------------------------------------------------------------------

#include <string>

//------------------------------------------------------------------------------

namespace FileFormat {
    enum FileFormat
    {
        Unknown,
        Mp3,
        Wav,
        Flac,
        Ogg,
        Opus,
        Raw,
        Dat,
        Json,
        Txt,
        Png
    };

    FileFormat getFormatViaSndfile(const std::string& filePath);
    FileFormat fromString(const std::string& name);
    std::string getFileExt(FileFormat file_format);
    std::string toString(FileFormat file_format);
    bool isSupported(FileFormat file_format);
    bool isAudioFormat(FileFormat file_format);
    bool isWaveformDataFormat(FileFormat file_format);
}

//------------------------------------------------------------------------------

#endif // #if !defined(INC_FILE_FORMAT_H)

//------------------------------------------------------------------------------
