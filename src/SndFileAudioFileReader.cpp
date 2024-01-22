//------------------------------------------------------------------------------
//
// Copyright 2013-2024 BBC Research and Development
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

#include "SndFileAudioFileReader.h"
#include "AudioProcessor.h"
#include "Error.h"
#include "FileUtil.h"
#include "Log.h"
#include "ProgressReporter.h"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>

//------------------------------------------------------------------------------

static void showInfo(std::ostream& stream, const SF_INFO& info)
{
    stream << "Frames: " << info.frames
           << "\nSample rate: " << info.samplerate << " Hz"
           << "\nChannels: " << info.channels
           << "\nFormat: 0x" << std::hex << info.format << std::dec
           << "\nSections: " << info.sections
           << "\nSeekable: " << (info.seekable ? "yes" : "no") << '\n';
}

//------------------------------------------------------------------------------

SndFileAudioFileReader::SndFileAudioFileReader() :
    input_file_(nullptr)
{
    memset(&info_, 0, sizeof(info_));
}

//------------------------------------------------------------------------------

SndFileAudioFileReader::~SndFileAudioFileReader()
{
    close();
}

//------------------------------------------------------------------------------

void SndFileAudioFileReader::configure(int channels, int sample_rate, const std::string& format)
{
    if (channels <= 0) {
        throwError("Invalid number of input channels: must be greater than zero");
    }

    if (sample_rate <= 0) {
        throwError("Invalid input sample rate: must be greater than zero");
    }

    info_.seekable = 0;
    info_.frames = 0;
    info_.sections = 0;

    info_.channels = channels;
    info_.format = SF_FORMAT_RAW;
    info_.samplerate = sample_rate;

    if (format == "s8") {
        info_.format |= SF_FORMAT_PCM_S8;
    }
    else if (format == "u8") {
        info_.format |= SF_FORMAT_PCM_U8;
    }
    else if (format == "s16le") {
        info_.format |= SF_FORMAT_PCM_16;
        info_.format |= SF_ENDIAN_LITTLE;
    }
    else if (format == "s16be") {
        info_.format |= SF_FORMAT_PCM_16;
        info_.format |= SF_ENDIAN_BIG;
    }
    else if (format == "s24le") {
        info_.format |= SF_FORMAT_PCM_24;
        info_.format |= SF_ENDIAN_LITTLE;
    }
    else if (format == "s24be") {
        info_.format |= SF_FORMAT_PCM_24;
        info_.format |= SF_ENDIAN_BIG;
    }
    else if (format == "s32le") {
        info_.format |= SF_FORMAT_PCM_32;
        info_.format |= SF_ENDIAN_LITTLE;
    }
    else if (format == "s32be") {
        info_.format |= SF_FORMAT_PCM_32;
        info_.format |= SF_ENDIAN_BIG;
    }
    else if (format == "f32le") {
        info_.format |= SF_FORMAT_FLOAT;
        info_.format |= SF_ENDIAN_LITTLE;
    }
    else if (format == "f32be") {
        info_.format |= SF_FORMAT_FLOAT;
        info_.format |= SF_ENDIAN_BIG;
    }
    else if (format == "f64le") {
        info_.format |= SF_FORMAT_DOUBLE;
        info_.format |= SF_ENDIAN_LITTLE;
    }
    else if (format == "f64be") {
        info_.format |= SF_FORMAT_DOUBLE;
        info_.format |= SF_ENDIAN_BIG;
    }
    else {
        throwError("Unsupported format: %1%", format);
    }
}

//------------------------------------------------------------------------------

bool SndFileAudioFileReader::open(const char* input_filename, bool show_info)
{
    assert(input_file_ == nullptr);

    if (FileUtil::isStdioFilename(input_filename)) {
        input_file_ = sf_open_fd(fileno(stdin), SFM_READ, &info_, 0);

        if (input_file_ == nullptr) {
            log(Error) << "Failed to read input: "
                       << sf_strerror(nullptr) << '\n';

            return false;
        }
    }
    else {
        input_file_ = sf_open(input_filename, SFM_READ, &info_);

        if (input_file_ == nullptr) {
            log(Error) << "Failed to read file: " << input_filename << '\n'
                       << sf_strerror(nullptr) << '\n';

            return false;
        }
    }

    log(Info) << "Input file: "
              << FileUtil::getInputFilename(input_filename) << '\n';

    if (show_info) {
        showInfo(log(Info), info_);
    }

    return true;
}

//------------------------------------------------------------------------------

void SndFileAudioFileReader::close()
{
    if (input_file_ != nullptr) {
        sf_close(input_file_);
        input_file_ = nullptr;
    }
}

//------------------------------------------------------------------------------

bool SndFileAudioFileReader::run(AudioProcessor& processor)
{
    if (input_file_ == nullptr) {
        return false;
    }

    ProgressReporter progress_reporter;

    const int BUFFER_SIZE = 16384;

    float float_buffer[BUFFER_SIZE];
    short input_buffer[BUFFER_SIZE];

    const int sub_type = info_.format & SF_FORMAT_SUBMASK;

    const bool is_floating_point = sub_type == SF_FORMAT_FLOAT ||
                                   sub_type == SF_FORMAT_DOUBLE;

    sf_count_t frames_to_read = BUFFER_SIZE / info_.channels;
    sf_count_t frames_read    = frames_to_read;

    sf_count_t total_frames_read = 0;

    bool success = processor.init(info_.samplerate, info_.channels, info_.frames, BUFFER_SIZE);

    if (success && processor.shouldContinue()) {
        progress_reporter.update(0.0, 0, info_.frames);

        while (success && frames_read == frames_to_read) {
            if (is_floating_point) {
                frames_read = sf_readf_float(
                    input_file_,
                    float_buffer,
                    frames_to_read
                );

                // Scale floating-point samples from [-1.0, 1.0] to 16-bit
                // integer range. Note: we don't use SFC_SET_SCALE_FLOAT_INT_READ
                // as this scales using the overall measured waveform peak
                // amplitude, resulting in an unwanted amplitude change.

                for (int i = 0; i < frames_read * info_.channels; ++i) {
                    input_buffer[i] = static_cast<short>(
                        float_buffer[i] * std::numeric_limits<short>::max()
                    );
                }
            }
            else {
                frames_read = sf_readf_short(
                    input_file_,
                    input_buffer,
                    frames_to_read
                );
            }

            success = processor.process(
                input_buffer,
                static_cast<int>(frames_read)
            );

            total_frames_read += frames_read;

            const double seconds =
                static_cast<double>(total_frames_read) /
                static_cast<double>(info_.samplerate);

            progress_reporter.update(seconds, total_frames_read, info_.frames);
        }

        log(Info) << "\nRead " << total_frames_read << " frames\n";

        processor.done();
    }

    close();

    return success;
}
