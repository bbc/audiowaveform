//------------------------------------------------------------------------------
//
// Copyright 2013-2023 BBC Research and Development
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

#include "WaveformBuffer.h"
#include "FileHandle.h"
#include "FileUtil.h"
#include "Log.h"

#include "pdjson/pdjson.h"

#include <boost/format.hpp>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

//------------------------------------------------------------------------------

static int32_t readInt32(std::istream& stream)
{
    int32_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));

    return value;
}

//------------------------------------------------------------------------------

static uint32_t readUInt32(std::istream& stream)
{
    uint32_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));

    return value;
}

//------------------------------------------------------------------------------

static int16_t readInt16(std::istream& stream)
{
    int16_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));

    return value;
}

//------------------------------------------------------------------------------

static int8_t readInt8(std::istream& stream)
{
    int8_t value;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));

    return value;
}

//------------------------------------------------------------------------------

static void writeInt32(std::ostream& stream, int32_t value)
{
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

//------------------------------------------------------------------------------

static void writeUInt32(std::ostream& stream, uint32_t value)
{
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

//------------------------------------------------------------------------------

static void writeInt8(std::ostream& stream, int8_t value)
{
    stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

//------------------------------------------------------------------------------

template <typename T>
static void writeVector(std::ostream& stream, const std::vector<T>& values)
{
    static_assert(std::is_integral<T>::value, "T must be integral type");

    stream.write(
        reinterpret_cast<const char*>(&values[0]),
        static_cast<std::streamsize>(values.size() * sizeof(T))
    );
}

//------------------------------------------------------------------------------

static void reportReadError(const char* filename, const char* message)
{
    log(Error) << "Failed to read data file: " << filename << '\n'
               << message << '\n';
}

//------------------------------------------------------------------------------

static void reportWriteError(const char* filename, const char* message)
{
    log(Error) << "Failed to write data file: " << filename << '\n'
               << message << '\n';
}

//------------------------------------------------------------------------------

const uint32_t FLAG_8_BIT = 0x00000001U;

//------------------------------------------------------------------------------

WaveformBuffer::WaveformBuffer() :
    sample_rate_(0),
    samples_per_pixel_(0),
    bits_(16),
    channels_(1)
{
}

//------------------------------------------------------------------------------

bool WaveformBuffer::load(const char* filename)
{
    bool success = true;

    std::ifstream file;
    std::istream* input;

    uint32_t size = 0;

    try {
        if (FileUtil::isStdioFilename(filename)) {
            input = &std::cin;
        }
        else {
            input = &file;
        }

        input->exceptions(std::ios::badbit | std::ios::failbit);

        if (!FileUtil::isStdioFilename(filename)) {
            file.open(filename, std::ios::in | std::ios::binary);
        }

        log(Info) << "Input file: "
                  << FileUtil::getInputFilename(filename) << '\n';

        const int32_t version = readInt32(*input);

        if (version != 1 && version != 2) {
            reportReadError(
                filename,
                boost::str(boost::format("Cannot load data file version: %1%") % version).c_str()
            );

            return false;
        }

        const uint32_t flags = readUInt32(*input);

        sample_rate_ = readInt32(*input);

        if (sample_rate_ < 1) {
            reportReadError(
                filename,
                boost::str(
                    boost::format("Invalid sample rate: %1% Hz, minimum 1 Hz") % sample_rate_
                ).c_str()
            );

            return false;
        }

        samples_per_pixel_ = readInt32(*input);

        if (samples_per_pixel_ < 2) {
            reportReadError(
                filename,
                boost::str(
                    boost::format("Invalid samples per pixel: %1%, minimum 2") % samples_per_pixel_
                ).c_str()
            );

            return false;
        }

        size = readUInt32(*input);

        if (version == 2) {
            channels_ = readInt32(*input);
        }
        else {
            channels_ = 1;
        }

        if (channels_ < 1 || channels_ > MAX_CHANNELS) {
            reportReadError(
                filename,
                boost::str(boost::format("Cannot load data file with %1% channels") % channels_).c_str()
            );

            return false;
        }

        if ((flags & FLAG_8_BIT) != 0) {
            bits_ = 8;

            for (uint32_t i = 0; i < size * channels_; ++i) {
                int8_t min_value = readInt8(*input);
                data_.push_back(static_cast<int16_t>(min_value * 256));

                int8_t max_value = readInt8(*input);
                data_.push_back(static_cast<int16_t>(max_value * 256));
            }
        }
        else {
            bits_ = 16;

            for (uint32_t i = 0; i < size * channels_; ++i) {
                int16_t min_value = readInt16(*input);
                data_.push_back(min_value);

                int16_t max_value = readInt16(*input);
                data_.push_back(max_value);
            }
        }

        log(Info) << "Channels: " << channels_
                  << "\nSample rate: " << sample_rate_ << " Hz"
                  << "\nBits: " << bits_
                  << "\nSamples per pixel: " << samples_per_pixel_
                  << "\nLength: " << getSize() << " points" << std::endl;


        file.clear();
    }
    catch (const std::exception& e) {

        // Note: Catching std::exception instead of std::ios::failure is a
        // workaround for a g++ v5 / v6 libstdc++ ABI bug.
        //
        // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66145
        // and http://stackoverflow.com/questions/38471518

        if (!file.eof()) {
            reportReadError(filename, strerror(errno));
            success = false;
        }
    }

    const int actual_size = getSize();

    if (size != static_cast<uint32_t>(actual_size)) {
        log(Info) << "Expected " << size << " points, read "
                  << actual_size << " min and max points\n";
    }

    return success;
}

//------------------------------------------------------------------------------

bool WaveformBuffer::loadJson(const char* filename)
{
    FileHandle file;

    if (!file.open(filename)) {
        return false;
    }

    log(Info) << "Input file: "
              << FileUtil::getInputFilename(filename) << '\n';

    json_stream json;
    json_open_stream(&json, file.get());

    enum json_type type = json_next(&json);
    bool error = false;
    bool found_version = false;
    bool found_data = false;
    bool found_length = false;
    int length = 0;

    enum class State {
        Initial,
        Key,
        Value,
        Data,
        DataValues,
        End
    } state = State::Initial;

    enum class Attribute {
        None,
        Version,
        Channels,
        SampleRate,
        SamplesPerPixel,
        Bits,
        Length
    } attribute = Attribute::None;

    std::string attribute_name;

    while (type != JSON_DONE && !error) {
        if (state == State::Initial) {
            if (type == JSON_OBJECT) {
                state = State::Key;
            }
            else {
                log(Error) << "Invalid JSON structure: expected object\n";
                error = true;
            }
        }
        else if (state == State::Key) {
            switch (type) {
                case JSON_STRING: {
                    std::string value = json_get_string(&json, 0);

                    if (value == "version") {
                        attribute = Attribute::Version;
                        state = State::Value;
                    }
                    else if (value == "channels") {
                        attribute = Attribute::Channels;
                        state = State::Value;
                    }
                    else if (value == "sample_rate") {
                        attribute = Attribute::SampleRate;
                        state = State::Value;
                    }
                    else if (value == "samples_per_pixel") {
                        attribute = Attribute::SamplesPerPixel;
                        state = State::Value;
                    }
                    else if (value == "bits") {
                        attribute = Attribute::Bits;
                        state = State::Value;
                    }
                    else if (value == "length") {
                        attribute = Attribute::Length;
                        state = State::Value;
                    }
                    else if (value == "data") {
                        state = State::Data;
                    }
                    else {
                        log(Error) << "Unexpected value: " << value << '\n';
                        error = true;
                    }

                    attribute_name = value;
                    break;
                }

                case JSON_OBJECT_END:
                    state = State::End;
                    break;

                default:
                    log(Error) << "Invalid JSON format at line "
                               << json_get_lineno(&json)
                               << ", column "
                               << json_get_position(&json) << "\n";
                    error = true;
                    break;
            }
        }
        else if (state == State::Value) {
            switch (type) {
                case JSON_NUMBER: {
                    double value = json_get_number(&json);

                    switch (attribute) {
                        case Attribute::Version: {
                            found_version = true;

                            int version = static_cast<int>(value);

                            if (version != 1 && version != 2) {
                                log(Error) << "Invalid version: expecting 1 or 2\n";
                                error = true;
                            }
                            break;
                        }

                        case Attribute::Channels:
                            channels_ = static_cast<int>(value);

                            if (channels_ <= 0) {
                                log(Error) << "Invalid channels: " << channels_ << '\n';
                                error = true;
                            }
                            break;

                        case Attribute::SampleRate:
                            sample_rate_ = static_cast<int>(value);

                            if (sample_rate_ <= 0) {
                                log(Error) << "Invalid sample rate: " << sample_rate_ << '\n';
                                error = true;
                            }
                            break;

                        case Attribute::SamplesPerPixel:
                            samples_per_pixel_ = static_cast<int>(value);

                            if (samples_per_pixel_ <= 0) {
                                log(Error) << "Invalid scale: " << samples_per_pixel_ << '\n';
                                error = true;
                            }
                            break;

                        case Attribute::Bits:
                            bits_ = static_cast<int>(value);

                            if (bits_ != 8 && bits_ != 16) {
                                log(Error) << "Invalid bits: expecting 8 or 16\n";
                                error = true;
                            }
                            break;

                        case Attribute::Length:
                            found_length = true;

                            length = static_cast<int>(value);

                            if (length < 0) {
                                log(Error) << "Invalid length: " << length << '\n';
                                error = true;
                            }
                            break;

                        default:
                            log(Error) << "Unexpected value: " << value << '\n';
                            error = true;
                            break;
                    }

                    state = State::Key;
                    break;
                }

                default:
                    log(Error) << "Expected " << attribute_name << " to be a number\n";
                    error = true;
                    break;
            }
        }
        else if (state == State::Data) {
            if (type == JSON_ARRAY) {
                state = State::DataValues;
                found_data = true;
            }
            else {
                log(Error) << "Expected data to be an array\n";
                error = true;
            }
        }
        else if (state == State::DataValues) {
            if (type == JSON_NUMBER) {
                double value = json_get_number(&json);

                if (bits_ == 8) {
                    if (value >= INT8_MIN && value <= INT8_MAX) {
                        data_.push_back(static_cast<short>(value * 256));
                    }
                    else {
                        log(Error) << "Data value out of range: " << value << '\n';
                        error = true;
                    }
                }
                else if (bits_ == 16) {
                    if (value >= INT16_MIN || value <= INT16_MAX) {
                        data_.push_back(static_cast<short>(value));
                    }
                    else {
                        log(Error) << "Data value out of range: " << value << '\n';
                        error = true;
                    }
                }
            }
            else if (type == JSON_ARRAY_END) {
                state = State::Key;
            }
            else {
                log(Error) << "Invalid JSON structure: the data array must only contain numbers\n";
                error = true;
            }
        }
        else if (state == State::End) {
            if (type != JSON_DONE) {
                log(Error) << "Invalid JSON structure: unexpected data\n";
                error = true;
            }
        }

        type = json_next(&json);
    }

    if (!error) {
        if (!found_version) {
            log(Error) << "Missing value: version\n";
            error = true;
        }
        else if (!found_length) {
            log(Error) << "Missing value: length\n";
            error = true;
        }
        else if (channels_ == 0) {
            log(Error) << "Missing value: channels\n";
            error = true;
        }
        else if (sample_rate_ == 0) {
            log(Error) << "Missing value: sample_rate\n";
            error = true;
        }
        else if (samples_per_pixel_ == 0) {
            log(Error) << "Missing value: samples_per_pixel\n";
            error = true;
        }
        else if (bits_ == 0) {
            log(Error) << "Missing value: bits\n";
            error = true;
        }
        else if (!found_data) {
            log(Error) << "Missing value: data\n";
            error = true;
        }
    }

    if (!error) {
        const int actual_size = getSize();

        log(Info) << "Channels: " << channels_
                  << "\nSample rate: " << sample_rate_ << " Hz"
                  << "\nBits: " << bits_
                  << "\nSamples per pixel: " << samples_per_pixel_
                  << "\nLength: " << actual_size << " points" << std::endl;

        if (length != actual_size) {
            log(Info) << "Expected " << length << " points, read "
                      << actual_size << " min and max points\n";
        }
    }

    json_close(&json);

    return !error;
}

//------------------------------------------------------------------------------

template<typename Writer>
static bool openOutputStream(const char* filename, bool binary, Writer writer)
{
    bool success = true;

    std::ofstream file;
    std::ostream* output;

    try {
        if (FileUtil::isStdioFilename(filename)) {
            output = &std::cout;
        }
        else {
            output = &file;
        }

        output->exceptions(std::ios::badbit | std::ios::failbit);

        if (!FileUtil::isStdioFilename(filename)) {
            std::ios::openmode openMode = std::ios::out;

            if (binary) {
                openMode |= std::ios::binary;
            }

            file.open(filename, openMode);
        }

        log(Info) << "Output file: "
                  << FileUtil::getOutputFilename(filename) << '\n';

        writer(*output);
    }
    catch (const std::exception&) {
        reportWriteError(filename, strerror(errno));
        success = false;
    }

    return success;
}

//------------------------------------------------------------------------------

bool WaveformBuffer::save(const char* filename, const int bits) const
{
    if (bits != 8 && bits != 16) {
        log(Error) << "Invalid bits: must be either 8 or 16\n";
        return false;
    }

    return openOutputStream(filename, true, [this, bits](std::ostream& output) {
        log(Info) << "Resolution: " << bits << " bits\n"
                  << "Channels: " << channels_ << std::endl;

        save(output, bits);
    });
}

//------------------------------------------------------------------------------

void WaveformBuffer::save(std::ostream& stream, int bits) const
{
    const int32_t version = channels_ == 1 ? 1 : 2;
    writeInt32(stream, version);

    uint32_t flags = 0;

    if (bits == 8) {
        flags |= FLAG_8_BIT;
    }

    writeUInt32(stream, flags);
    writeInt32(stream, sample_rate_);
    writeInt32(stream, samples_per_pixel_);

    const int size = getSize();

    writeUInt32(stream, static_cast<uint32_t>(size));

    if (version == 2) {
        writeInt32(stream, channels_);
    }

    if ((flags & FLAG_8_BIT) != 0) {
        for (int i = 0; i < size; ++i) {
            for (int channel = 0; channel < channels_; ++channel) {
                int8_t min_value = static_cast<int8_t>(getMinSample(channel, i) / 256);
                writeInt8(stream, min_value);

                int8_t max_value = static_cast<int8_t>(getMaxSample(channel, i) / 256);
                writeInt8(stream, max_value);
            }
        }
    }
    else {
        writeVector(stream, data_);
    }
}

//------------------------------------------------------------------------------

bool WaveformBuffer::saveAsText(const char* filename, int bits) const
{
    if (bits != 8 && bits != 16) {
        log(Error) << "Invalid bits: must be either 8 or 16\n";
        return false;
    }

    return openOutputStream(filename, false, [this, bits](std::ostream& output) {
        saveAsText(output, bits);
    });
}

//------------------------------------------------------------------------------

void WaveformBuffer::saveAsText(std::ostream& stream, int bits) const
{
    const int size = getSize();

    if (bits == 8) {
        for (int i = 0; i < size; ++i) {
            for (int channel = 0; channel < channels_; ++channel) {
                const int min_value = getMinSample(channel, i) / 256;
                const int max_value = getMaxSample(channel, i) / 256;

                if (channel > 0) {
                    stream << ',';
                }

                stream << min_value << ',' << max_value;
            }

            stream << '\n';
        }
    }
    else {
        for (int i = 0; i < size; ++i) {
            for (int channel = 0; channel < channels_; ++channel) {
                if (channel > 0) {
                    stream << ',';
                }

                stream << getMinSample(channel, i) << ','
                       << getMaxSample(channel, i);
            }

            stream << '\n';
        }
    }
}

//------------------------------------------------------------------------------

bool WaveformBuffer::saveAsJson(const char* filename, const int bits) const
{
    if (bits != 8 && bits != 16) {
        log(Error) << "Invalid bits: must be either 8 or 16\n";
        return false;
    }

    return openOutputStream(filename, false, [this, bits](std::ostream& output) {
        saveAsJson(output, bits);
    });
}

//------------------------------------------------------------------------------

static void writeAsJsonArray(
    std::ostream& stream,
    const std::vector<short>& data,
    int divisor)
{
    auto i = data.begin();

    stream << '[';

    if (i != data.end()) {
        stream << (*i / divisor);
        ++i;
    }

    for (; i != data.end(); ++i) {
        stream << ',' << (*i / divisor);
    }

    stream << ']';
}

//------------------------------------------------------------------------------

void WaveformBuffer::saveAsJson(std::ostream& stream, int bits) const
{
    const int size = getSize();
    const int version = 2;

    stream << "{\"version\":" << version
           << ",\"channels\":" << channels_
           << ",\"sample_rate\":" << sample_rate_
           << ",\"samples_per_pixel\":" << samples_per_pixel_
           << ",\"bits\":" << bits
           << ",\"length\":" << size
           << ",\"data\":";

    if (bits == 8) {
        writeAsJsonArray(stream, data_, 256);
    }
    else {
        writeAsJsonArray(stream, data_, 1);
    }

    stream << "}\n";
}

//------------------------------------------------------------------------------
