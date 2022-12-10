//------------------------------------------------------------------------------
//
// Copyright 2022 BBC Research and Development
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

#include "VectorAudioFileReader.h"
#include "AudioProcessor.h"
#include "Log.h"
#include "ProgressReporter.h"

#include <iostream>

//------------------------------------------------------------------------------

VectorAudioFileReader::VectorAudioFileReader(
    const std::vector<short>& samples,
    int sample_rate,
    int channels) :
    samples_(samples),
    sample_rate_(sample_rate),
    channels_(channels)
{
}

//------------------------------------------------------------------------------

VectorAudioFileReader::~VectorAudioFileReader()
{
}

//------------------------------------------------------------------------------

bool VectorAudioFileReader::open(const char* /* input_filename */, bool /* show_info */)
{
    return true;
}

//------------------------------------------------------------------------------

void VectorAudioFileReader::close()
{
}

//------------------------------------------------------------------------------

bool VectorAudioFileReader::run(AudioProcessor& processor)
{
    ProgressReporter progress_reporter;

    const size_t BUFFER_SIZE = 16384;

    const size_t total_frames = samples_.size() / channels_;
    size_t frames_to_read = total_frames;
    size_t index = 0;
    size_t total_frames_read = 0;

    bool success = processor.init(sample_rate_, channels_, total_frames, BUFFER_SIZE);

    if (success && processor.shouldContinue()) {
        progress_reporter.update(0.0, 0, total_frames);

        while (success && frames_to_read > 0) {
            size_t max_frames = BUFFER_SIZE / channels_;
            size_t frames = frames_to_read > max_frames ? max_frames : frames_to_read;

            success = processor.process(
                &samples_[index],
                static_cast<int>(frames)
            );

            total_frames_read += frames;
            index += frames * channels_;
            frames_to_read -= frames;

            double seconds = static_cast<double>(total_frames_read) / static_cast<double>(sample_rate_);

            progress_reporter.update(seconds, total_frames_read, total_frames);
        }

        log(Info) << '\n';

        processor.done();
    }

    close();

    return success;
}

//------------------------------------------------------------------------------
