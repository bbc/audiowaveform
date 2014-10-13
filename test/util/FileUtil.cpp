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

#include "FileUtil.h"

#include <fstream>

//------------------------------------------------------------------------------

namespace FileUtil {

//------------------------------------------------------------------------------

boost::filesystem::path getTempFilename(const char* ext)
{
    char* basename = tempnam(nullptr, "aud");

    boost::filesystem::path filename = basename;

    if (ext != nullptr) {
        filename.replace_extension(ext);
    }

    free(basename);

    return filename;
}

//------------------------------------------------------------------------------

std::vector<uint8_t> readFile(const boost::filesystem::path& filename)
{
    boost::uintmax_t size = boost::filesystem::file_size(filename);

    std::vector<uint8_t> data(size);

    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    file.read(reinterpret_cast<char*>(&data[0]), size);

    return data;
}

//------------------------------------------------------------------------------

std::string readTextFile(const boost::filesystem::path& filename)
{
    std::ifstream stream(filename.c_str(), std::ios::in);

    std::string str;

    char buffer[1024];

    while (!stream.eof()) {
        stream.read(buffer, sizeof(buffer));
        std::streamsize count = stream.gcount();

        str.append(buffer, count);
    }

    return str;
}

//------------------------------------------------------------------------------

} // namespace FileUtil

//------------------------------------------------------------------------------
