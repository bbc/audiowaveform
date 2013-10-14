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

#include "SndFileAudioFileReader.h"
#include "AudioProcessor.h"
#include "Streams.h"

#include <cstring>
#include <iomanip>
#include <iostream>

//------------------------------------------------------------------------------

static void dumpInfo(std::ostream& stream, const SF_INFO& info)
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

bool SndFileAudioFileReader::open(const char* input_filename)
{
    input_file_ = sf_open(input_filename, SFM_READ, &info_);

    if (input_file_ != nullptr) {
        output_stream << "Input file: " << input_filename << std::endl;

        dumpInfo(output_stream, info_);
    }
    else {
        error_stream << "Failed to read file: " << input_filename << '\n'
                     << sf_strerror(input_file_) << '\n';
    }

    return input_file_ != nullptr;
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

    const int BUFFER_SIZE = 16384;

    short input_buffer[BUFFER_SIZE];

    sf_count_t frames_to_read = BUFFER_SIZE / info_.channels;
    sf_count_t frames_read    = frames_to_read;

    sf_count_t total_frames_read = 0;

    bool success = true;

    success = processor.init(info_.samplerate, info_.channels, BUFFER_SIZE);

    if (success) {
        showProgress(0, info_.frames);

        while (success && frames_read == frames_to_read) {
            frames_read = sf_readf_short(
                input_file_,
                input_buffer,
                frames_to_read
            );

            success = processor.process(
                input_buffer,
                static_cast<int>(frames_read)
            );

            total_frames_read += frames_read;

            showProgress(total_frames_read, info_.frames);
        }

        output_stream << "\nRead " << total_frames_read << " frames\n";

        processor.done();
    }

    close();

    return success;
}

//------------------------------------------------------------------------------
