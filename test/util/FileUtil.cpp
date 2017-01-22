//------------------------------------------------------------------------------
//
// Copyright 2014-2017 BBC Research and Development
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

#include <boost/numeric/conversion/cast.hpp>

//------------------------------------------------------------------------------

namespace FileUtil {

//------------------------------------------------------------------------------

boost::filesystem::path getTempFilename(const char* ext)
{
    boost::filesystem::path filename;

    filename /= boost::filesystem::temp_directory_path();
    filename /= boost::filesystem::unique_path();

    if (ext != nullptr) {
        filename.replace_extension(ext);
    }

    return filename;
}

//------------------------------------------------------------------------------

std::vector<uint8_t> readFile(const boost::filesystem::path& filename)
{
    const auto file_size = boost::filesystem::file_size(filename);

    const auto size = boost::numeric_cast<std::streamsize>(file_size);

    std::vector<uint8_t> data(static_cast<std::size_t>(size));

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

        str.append(buffer, static_cast<std::string::size_type>(count));
    }

    return str;
}

//------------------------------------------------------------------------------

} // namespace FileUtil

//------------------------------------------------------------------------------
