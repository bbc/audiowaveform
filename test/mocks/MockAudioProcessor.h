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

#if !defined(INC_MOCK_AUDIO_PROCESSOR_H)
#define INC_MOCK_AUDIO_PROCESSOR_H

//------------------------------------------------------------------------------

#include "AudioProcessor.h"

#include "gmock/gmock.h"

//------------------------------------------------------------------------------

class MockAudioProcessor : public AudioProcessor
{
    public:
        MOCK_METHOD3(init, bool(int sample_rate, int channels, int buffer_size));
        MOCK_METHOD2(process, bool(const short* buffer, int frame_count));
        MOCK_METHOD0(done, void());
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_MOCK_AUDIO_PROCESSOR_H)

//------------------------------------------------------------------------------
