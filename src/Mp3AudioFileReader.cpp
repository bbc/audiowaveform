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
//
// This file contains code adapted from madlld, (c) 2001--2004 Bertrand Petit
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the author nor the names of its contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//------------------------------------------------------------------------------

#include "Mp3AudioFileReader.h"
#include "AudioProcessor.h"
#include "BStdFile.h"
#include "Error.h"
#include "Streams.h"

#include <sys/stat.h>
#include <id3tag.h>
#include <mad.h>

#include <climits>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <iostream>

//------------------------------------------------------------------------------

class MadStream : public mad_stream
{
    public:
        MadStream();
        ~MadStream();
};

//------------------------------------------------------------------------------

MadStream::MadStream()
{
    mad_stream_init(this);
}

//------------------------------------------------------------------------------

MadStream::~MadStream()
{
    mad_stream_finish(this);
}

//------------------------------------------------------------------------------

class MadFrame : public mad_frame
{
    public:
        MadFrame();
        ~MadFrame();
};

//------------------------------------------------------------------------------

MadFrame::MadFrame()
{
    mad_frame_init(this);
}

//------------------------------------------------------------------------------

MadFrame::~MadFrame()
{
    mad_frame_finish(this);
}

//------------------------------------------------------------------------------

class MadSynth : public mad_synth
{
    public:
        MadSynth();
        ~MadSynth();
};

//------------------------------------------------------------------------------

MadSynth::MadSynth()
{
    mad_synth_init(this);
}

//------------------------------------------------------------------------------

MadSynth::~MadSynth()
{
    mad_synth_finish(this);
}

//------------------------------------------------------------------------------

const int INPUT_BUFFER_SIZE  = 5 * 8192;
const int OUTPUT_BUFFER_SIZE = 8192;

//------------------------------------------------------------------------------

// Print human readable information about an audio MPEG frame.

static void dumpInfo(std::ostream& stream, const struct mad_header& header)
{
    const char* layer;
    const char* mode;
    const char* emphasis;

    // Convert the layer number to its printed representation.

    switch (header.layer) {
        case MAD_LAYER_I:
            layer = "I";
            break;

        case MAD_LAYER_II:
            layer = "II";
            break;

        case MAD_LAYER_III:
            layer = "III";
            break;

        default:
            layer = "(unexpected layer value)";
            break;
    }

    // Convert the audio mode to its printed representation.

    switch (header.mode) {
        case MAD_MODE_SINGLE_CHANNEL:
            mode = "single channel";
            break;

        case MAD_MODE_DUAL_CHANNEL:
            mode = "dual channel";
            break;

        case MAD_MODE_JOINT_STEREO:
            mode = "joint (MS/intensity) stereo";
            break;

        case MAD_MODE_STEREO:
            mode = "normal LR stereo";
            break;

        default:
            mode = "(unexpected mode value)";
            break;
    }

    // Convert the emphasis to its printed representation. Note that the
    // MAD_EMPHASIS_RESERVED enumeration value appeared in libmad version
    // 0.15.0b.

    switch (header.emphasis) {
        case MAD_EMPHASIS_NONE:
            emphasis = "no";
            break;

        case MAD_EMPHASIS_50_15_US:
            emphasis = "50/15 us";
            break;

        case MAD_EMPHASIS_CCITT_J_17:
            emphasis = "CCITT J.17";
            break;

#if (MAD_VERSION_MAJOR>=1) || \
    ((MAD_VERSION_MAJOR==0) && (MAD_VERSION_MINOR>=15))

        case MAD_EMPHASIS_RESERVED:
            emphasis = "reserved(!)";
            break;

#endif

        default:
            emphasis = "(unexpected emphasis value)";
            break;
    }

    stream << "Format: Audio MPEG layer " << layer << " stream"
           << "\nBit rate: " << header.bitrate << " kbit/s"
           << "\nCRC: " << ((header.flags & MAD_FLAG_PROTECTION) ? "yes" : "no")
           << "\nMode: " << mode
           << "\nEmphasis: " << emphasis
           << "\nSample rate: " << header.samplerate << " Hz\n";
}

//------------------------------------------------------------------------------

// Converts a sample from libmad's fixed point number format to a signed short
// (16 bits).

static short MadFixedToSshort(mad_fixed_t fixed)
{
    // A fixed point number is formed of the following bit pattern:

    //
    // SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    // MSB                          LSB
    // S ==> Sign (0 is positive, 1 is negative)
    // W ==> Whole part bits
    // F ==> Fractional part bits
    //
    // This pattern contains MAD_F_FRACBITS fractional bits, one should alway
    // use this macro when working on the bits of a fixed point number. It is
    // not guaranteed to be constant over the different platforms supported by
    // libmad.
    //
    // The signed short value is formed, after clipping, by the least
    // significant whole part bit, followed by the 15 most significant
    // fractional part bits. Warning: this is a quick and dirty way to compute
    // the 16-bit number, madplay includes much better algorithms.

    // Clipping
    if (fixed >= MAD_F_ONE) {
        return SHRT_MAX;
    }

    if (fixed <= -MAD_F_ONE) {
        return -SHRT_MAX;
    }

    // Conversion
    fixed >>= (MAD_F_FRACBITS - 15);

    return static_cast<short>(fixed);
}

//------------------------------------------------------------------------------

Mp3AudioFileReader::Mp3AudioFileReader() :
    file_(nullptr),
    file_size_(0)
{
}

//------------------------------------------------------------------------------

Mp3AudioFileReader::~Mp3AudioFileReader()
{
    close();
}

//------------------------------------------------------------------------------

bool Mp3AudioFileReader::open(const char* filename)
{
    file_ = fopen(filename, "rb");

    if (file_ != nullptr) {
        output_stream << "Input file: " << filename << std::endl;

        // Get the file size, so we can show a progress indicator.

        if (!getFileSize()) {
            error_stream << "Failed to determine file size: "
                         << strerror(errno) << '\n';

            close();

            return false;
        }

        if (!skipId3Tags()) {
            error_stream << "Failed to read file: " << filename << '\n'
                         << strerror(errno) << '\n';

            close();

            return false;
        }
    }
    else {
        error_stream << "Failed to read file: " << filename << '\n'
                     << strerror(errno) << '\n';
    }

    return file_ != nullptr;
}

//------------------------------------------------------------------------------

void Mp3AudioFileReader::close()
{
    if (file_ != nullptr) {
        fclose(file_);
        file_ = nullptr;
    }
}

//------------------------------------------------------------------------------

bool Mp3AudioFileReader::getFileSize()
{
    struct stat stat_buf;

    int descriptor = fileno(file_);

    if (descriptor == -1 || fstat(descriptor, &stat_buf) != 0) {
        return false;
    }

    file_size_ = stat_buf.st_size;

    return true;
}

//------------------------------------------------------------------------------

bool Mp3AudioFileReader::skipId3Tags()
{
    assert(file_ != nullptr);

    unsigned char buffer[ID3_TAG_QUERYSIZE];
    const size_t items_read = fread(buffer, ID3_TAG_QUERYSIZE, 1, file_);

    if (items_read == 0) {
        return false;
    }

    long length = id3_tag_query(buffer, ID3_TAG_QUERYSIZE);

    if (length < 0) {
        length = 0;
    }

    return fseek(file_, length, SEEK_SET) == 0;
}

//------------------------------------------------------------------------------

bool Mp3AudioFileReader::run(AudioProcessor& processor)
{
    if (file_ == nullptr) {
        return false;
    }

    enum {
        STATUS_OK,
        STATUS_INIT_ERROR,
        STATUS_READ_ERROR,
        STATUS_PROCESS_ERROR
    } status = STATUS_OK;

    unsigned char input_buffer[INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
    unsigned char* guard_ptr = nullptr;
    unsigned long frame_count = 0;

    short output_buffer[OUTPUT_BUFFER_SIZE];
    short* output_ptr = output_buffer;
    const short* const output_buffer_end = output_buffer + OUTPUT_BUFFER_SIZE;

    int channels = 0;

    // Decoding options can here be set in the options field of the stream
    // structure.

    // {1} When decoding from a file we need to know when the end of the file is
    // reached at the same time as the last bytes are read (see also the comment
    // marked {3} below). Neither the standard C fread() function nor the POSIX
    // read() system call provides this feature. We thus need to perform our
    // reads through an interface having this feature, this is implemented here
    // by the bstdfile.c module.

    BStdFile bstd_file(file_);

    // Initialize the structures used by libmad.
    MadStream stream;
    MadFrame frame;
    MadSynth synth;

    mad_timer_t timer;
    mad_timer_reset(&timer);

    // This is the decoding loop.

    for (;;) {
        // The input bucket must be filled if it becomes empty or if it's the
        // first execution of the loop.

        if (stream.buffer == nullptr || stream.error == MAD_ERROR_BUFLEN) {
            size_t read_size;
            size_t remaining;
            unsigned char* read_start;

            // {2} libmad may not consume all bytes of the input buffer. If the
            // last frame in the buffer is not wholly contained by it, then that
            // frame's start is pointed by the next_frame member of the stream
            // structure. This common situation occurs when mad_frame_decode()
            // fails, sets the stream error code to MAD_ERROR_BUFLEN, and sets
            // the next_frame pointer to a non-NULL value. (See also the comment
            // marked {4} below.)
            //
            // When this occurs, the remaining unused bytes must be put back at
            // the beginning of the buffer and taken in account before refilling
            // the buffer. This means that the input buffer must be large enough
            // to hold a whole frame at the highest observable bit-rate
            // (currently 448 kb/s). XXX=XXX Is 2016 bytes the size of the
            // largest frame? (448000*(1152/32000))/8

            if (stream.next_frame != nullptr) {
                remaining = stream.bufend - stream.next_frame;
                memmove(input_buffer, stream.next_frame, remaining);
                read_start = input_buffer + remaining;
                read_size  = INPUT_BUFFER_SIZE - remaining;
            }
            else {
                read_size  = INPUT_BUFFER_SIZE;
                read_start = input_buffer;
                remaining = 0;
            }

            // Fill-in the buffer. If an error occurs print a message and leave
            // the decoding loop. If the end of stream is reached we also leave
            // the loop but the return status is left untouched.

            read_size = bstd_file.read(read_start, 1, read_size);

            if (read_size <= 0) {
                if (ferror(file_)) {
                    error_stream << "\nRead error on bit-stream: "
                                 << strerror(errno) << '\n';
                    status = STATUS_READ_ERROR;
                }

                break;
            }

            // {3} When decoding the last frame of a file, it must be followed
            // by MAD_BUFFER_GUARD zero bytes if one wants to decode that last
            // frame. When the end of file is detected we append that quantity
            // of bytes at the end of the available data. Note that the buffer
            // can't overflow as the guard size was allocated but not used the
            // the buffer management code. (See also the comment marked {1}.)
            //
            // In a message to the mad-dev mailing list on May 29th, 2001, Rob
            // Leslie explains the guard zone as follows:
            //
            //    "The reason for MAD_BUFFER_GUARD has to do with the way
            //    decoding is performed. In Layer III, Huffman decoding may
            //    inadvertently read a few bytes beyond the end of the buffer in
            //    the case of certain invalid input. This is not detected until
            //    after the fact. To prevent this from causing problems, and
            //    also to ensure the next frame's main_data_begin pointer is
            //    always accessible, MAD requires MAD_BUFFER_GUARD (currently 8)
            //    bytes to be present in the buffer past the end of the current
            //    frame in order to decode the frame."

            if (bstd_file.eof()) {
                guard_ptr = read_start + read_size;
                memset(guard_ptr, 0, MAD_BUFFER_GUARD);
                read_size += MAD_BUFFER_GUARD;
            }

            // Pipe the new buffer content to libmad's stream decoder facility.
            mad_stream_buffer(&stream, input_buffer, read_size + remaining);
            stream.error = MAD_ERROR_NONE;
        }

        // Decode the next MPEG frame. The streams is read from the buffer, its
        // constituents are broken down and stored the the frame structure,
        // ready for examination/alteration or PCM synthesis. Decoding options
        // are carried in the frame structure from the stream structure.
        //
        // Error handling: mad_frame_decode() returns a non zero value when an
        // error occurs. The error condition can be checked in the error member
        // of the stream structure. A mad error is recoverable or fatal, the
        // error status is checked with the MAD_RECOVERABLE macro.
        //
        // {4} When a fatal error is encountered all decoding activities shall
        // be stopped, except when a MAD_ERROR_BUFLEN is signaled. This
        // condition means that the mad_frame_decode() function needs more input
        // to complete its work. One shall refill the buffer and repeat the
        // mad_frame_decode() call. Some bytes may be left unused at the end of
        // the buffer if those bytes forms an incomplete frame. Before
        // refilling, the remaining bytes must be moved to the beginning of the
        // buffer and used for input for the next mad_frame_decode() invocation.
        // (See the comments marked {2} earlier for more details.)
        //
        // Recoverable errors are caused by malformed bit-streams, in this case
        // one can call again mad_frame_decode() in order to skip the faulty
        // part and re-sync to the next frame.

        if (mad_frame_decode(&frame, &stream)) {
            if (MAD_RECOVERABLE(stream.error)) {
                // Do not print a message if the error is a loss of
                // synchronization and this loss is due to the end of stream
                // guard bytes. (See the comment marked {3} above for more
                // information about guard bytes.)

                if (stream.error != MAD_ERROR_LOSTSYNC ||
                    stream.this_frame != guard_ptr) {

                    // For any MP3 file we typically see two errors in the
                    // first frame processed:
                    // - lost synchronization
                    // - reserved header layer value
                    // This seems to be OK, so don't print these

                    if (frame_count != 0) {
                        error_stream << "\nRecoverable frame level error: "
                                     << mad_stream_errorstr(&stream) << '\n';
                    }
                }

                continue;
            }
            else {
                if (stream.error == MAD_ERROR_BUFLEN) {
                    continue;
                }
                else {
                    error_stream << "\nUnrecoverable frame level error: "
                                 << mad_stream_errorstr(&stream) << '\n';
                    status = STATUS_READ_ERROR;
                    break;
                }
            }
        }

        // Display the characteristics of the stream's first frame. The first
        // frame is representative of the entire stream.

        if (frame_count == 0) {
            const int sample_rate = frame.header.samplerate;
            channels = MAD_NCHANNELS(&frame.header);

            dumpInfo(output_stream, frame.header);

            if (!processor.init(sample_rate, channels, OUTPUT_BUFFER_SIZE)) {
                status = STATUS_PROCESS_ERROR;
                break;
            }

            showProgress(0, file_size_);
        }

        // Accounting. The computed frame duration is in the frame header
        // structure. It is expressed as a fixed point number whole data type is
        // mad_timer_t. It is different from the samples fixed point format and
        // unlike it, it can't directly be added or subtracted. The timer module
        // provides several functions to operate on such numbers. Be careful
        // there, as some functions of libmad's timer module receive some of
        // their mad_timer_t arguments by value!

        frame_count++;
        mad_timer_add(&timer, frame.header.duration);

        // Once decoded the frame is synthesized to PCM samples. No errors are
        // reported by mad_synth_frame();

        mad_synth_frame(&synth, &frame);

        // Synthesized samples must be converted from libmad's fixed point
        // number to the consumer format. Here we use signed 16 bit integers on
        // two channels. Integer samples are temporarily stored in a buffer that
        // is flushed when full.

        for (int i = 0; i < synth.pcm.length; i++) {
            // Left channel
            short sample = MadFixedToSshort(synth.pcm.samples[0][i]);

            *output_ptr++ = sample;

            // Right channel. If the decoded stream is monophonic then the right
            // output channel is the same as the left one.

            if (MAD_NCHANNELS(&frame.header) == 2) {
                sample = MadFixedToSshort(synth.pcm.samples[1][i]);
                *output_ptr++ = sample;
            }

            // Flush the output buffer if it is full

            if (output_ptr == output_buffer_end) {
                long pos = ftell(file_);

                showProgress(pos, file_size_);

                bool success = processor.process(
                    output_buffer,
                    OUTPUT_BUFFER_SIZE / channels
                );

                if (!success) {
                    status = STATUS_PROCESS_ERROR;
                    break;
                }

                output_ptr = output_buffer;
            }
        }
    }

    // If the output buffer is not empty and no error occurred during the last
    // write, then flush it.

    if (output_ptr != output_buffer && status != STATUS_PROCESS_ERROR) {
        int buffer_size = static_cast<int>(output_ptr - output_buffer);

        bool success = processor.process(output_buffer, buffer_size / channels);

        if (!success) {
            status = STATUS_PROCESS_ERROR;
        }
    }

    // Accounting report if no error occurred.

    if (status == STATUS_OK) {
        // Report 100% done.
        showProgress(file_size_, file_size_);

        char buffer[80];

        // The duration timer is converted to a human readable string with the
        // versatile, but still constrained mad_timer_string() function, in a
        // fashion not unlike strftime(). The main difference is that the timer
        // is broken into several values according some of its arguments. The
        // units and fracunits arguments specify the intended conversion to be
        // executed.
        //
        // The conversion unit (MAD_UNIT_MINUTES in our example) also specify
        // the order and kind of conversion specifications that can be used in
        // the format string.
        //
        // It is best to examine libmad's timer.c source-code for details of the
        // available units, fraction of units, their meanings, the format
        // arguments, etc.

        mad_timer_string(timer, buffer, "%lu:%02lu.%03u",
            MAD_UNITS_MINUTES, MAD_UNITS_MILLISECONDS, 0);

        output_stream << "\nFrames decoded: " << frame_count
                      << " (" << buffer << ")\n";
    }

    processor.done();

    close();

    return status == STATUS_OK;
}

//------------------------------------------------------------------------------
