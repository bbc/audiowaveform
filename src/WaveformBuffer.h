//------------------------------------------------------------------------------
//
// Copyright 2013-2020 BBC Research and Development
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

#if !defined(INC_WAVEFORM_BUFFER_H)
#define INC_WAVEFORM_BUFFER_H

//------------------------------------------------------------------------------

#include <iosfwd>
#include <vector>

//------------------------------------------------------------------------------

class WaveformBuffer
{
    public:
        WaveformBuffer();

        WaveformBuffer(const WaveformBuffer& buffer) = delete;
        WaveformBuffer& operator=(const WaveformBuffer& buffer) = delete;

    public:
        static const int MAX_CHANNELS = 24;

    public:
        void setSampleRate(int sample_rate)
        {
            sample_rate_ = sample_rate;
        }

        void setSamplesPerPixel(int samples_per_pixel)
        {
            samples_per_pixel_ = samples_per_pixel;
        }

        int getSampleRate() const { return sample_rate_; }
        int getSamplesPerPixel() const { return samples_per_pixel_; }

        int getBits() const { return bits_; }

        int getChannels() const { return channels_; }

        void setChannels(int channels)
        {
            channels_ = channels;
        }

        int getSize() const
        {
            return static_cast<int>(data_.size() / (2 * channels_));
        }

        void setSize(int size)
        {
            data_.resize(static_cast<size_type>(size * 2 * channels_));
        }

        short getMinSample(int channel, int index) const
        {
            const size_type offset = static_cast<size_type>(
                (index * channels_ + channel) * 2
            );

            return data_[offset];
        }

        short getMaxSample(int channel, int index) const
        {
            const size_type offset = static_cast<size_type>(
                (index * channels_ + channel) * 2 + 1
            );

            return data_[offset];
        }

        void appendSamples(short min, short max)
        {
            data_.push_back(min);
            data_.push_back(max);
        }

        void setSamples(int channel, int index, short min, short max)
        {
            const size_type offset = static_cast<size_type>(
                (index * channels_ + channel) * 2
            );

            data_[offset] = min;
            data_[offset + 1] = max;
        }

        bool load(const char* filename);
        bool save(const char* filename, int bits = 16) const;
        bool saveAsText(const char* filename, int bits = 16) const;
        bool saveAsJson(const char* filename, int bits = 16) const;

    private:
        void save(std::ostream& stream, int bits) const;
        void saveAsText(std::ostream& stream, int bits) const;
        void saveAsJson(std::ostream& stream, int bits) const;

    private:
        int sample_rate_;
        int samples_per_pixel_;
        int bits_;
        int channels_;

        typedef std::vector<short> vector_type;
        typedef vector_type::size_type size_type;
        vector_type data_;
};

//------------------------------------------------------------------------------

#endif // #if !defined(INC_WAVEFORM_BUFFER_H)

//------------------------------------------------------------------------------
