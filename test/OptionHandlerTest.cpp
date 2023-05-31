//------------------------------------------------------------------------------
//
// Copyright 2014-2023 BBC Research and Development
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

#include "OptionHandler.h"
#include "FileFormat.h"
#include "Options.h"
#include "Array.h"
#include "util/FileDeleter.h"
#include "util/FileUtil.h"

#include "gmock/gmock.h"

#include <gd.h>
#include <string.h>

//------------------------------------------------------------------------------

using testing::StartsWith;
using testing::EndsWith;
using testing::Eq;
using testing::Ne;
using testing::NotNull;
using testing::StrEq;
using testing::HasSubstr;
using testing::Test;

//------------------------------------------------------------------------------

class OptionHandlerTest : public Test
{
    public:
        OptionHandlerTest()
        {
        }
};

//------------------------------------------------------------------------------

enum class InputMethod {
    StdIn,
    Pipe,
    File
};

enum class OutputMethod {
    StdOut,
    File
};

//------------------------------------------------------------------------------

template<typename T>
static void compare(
    const std::vector<T>& test_data,
    const std::vector<T>& ref_data)
{
    ASSERT_THAT(test_data.size(), Eq(ref_data.size()));

    const auto size = test_data.size();

    for (std::size_t i = 0; i != size; ++i) {
        ASSERT_THAT(test_data[i], Eq(ref_data[i]));
    }
}

//------------------------------------------------------------------------------

static void compareFiles(
    const boost::filesystem::path& test_filename,
    const boost::filesystem::path& ref_filename)
{
    std::vector<uint8_t> test_data = FileUtil::readFile(test_filename);
    std::vector<uint8_t> ref_data  = FileUtil::readFile(ref_filename);

    compare(test_data, ref_data);
}

//------------------------------------------------------------------------------

static gdImagePtr openImageFile(const boost::filesystem::path& filename)
{
    gdImagePtr image = nullptr;

    FILE* file = fopen(filename.c_str(), "rb");

    if (file != nullptr) {
        image = gdImageCreateFromPng(file);

        fclose(file);
        file = nullptr;
    }

    return image;
}

//------------------------------------------------------------------------------

static void compareImages(
    gdImagePtr test_image,
    gdImagePtr ref_image)
{
    const int test_width = gdImageSX(test_image);
    const int ref_width  = gdImageSX(ref_image);

    const int test_height = gdImageSY(test_image);
    const int ref_height  = gdImageSY(ref_image);

    ASSERT_THAT(test_width,  Eq(ref_width));
    ASSERT_THAT(test_height, Eq(ref_height));

    for (int y = 0; y < ref_height; ++y) {
        for (int x = 0; x < ref_width; ++x) {
            const int test_pixel = gdImageGetPixel(test_image, x, y);
            const int ref_pixel  = gdImageGetPixel(ref_image,  x, y);

            ASSERT_THAT(test_pixel, Eq(ref_pixel));
        }
    }
}

//------------------------------------------------------------------------------

static void compareImageFiles(
    const boost::filesystem::path& test_filename,
    const boost::filesystem::path& ref_filename)
{
    gdImagePtr test_image = openImageFile(test_filename);
    ASSERT_THAT(test_image, NotNull());

    gdImagePtr ref_image = openImageFile(ref_filename);
    ASSERT_THAT(ref_image, NotNull());

    compareImages(test_image, ref_image);

    gdImageDestroy(test_image);
    gdImageDestroy(ref_image);
}

//------------------------------------------------------------------------------

static void runTest(
    InputMethod input_method,
    const char* input_filename,
    FileFormat::FileFormat input_file_format,
    OutputMethod output_method,
    FileFormat::FileFormat output_file_format,
    const std::vector<const char*>* args,
    bool should_succeed,
    const char* reference_filename = nullptr,
    const char* error_message = nullptr)
{
    boost::filesystem::path input_pathname = "../test/data";
    input_pathname /= input_filename;

    const boost::filesystem::path output_pathname =
        FileUtil::getTempFilename(FileFormat::getFileExt(output_file_format).c_str());

    const boost::filesystem::path stdout_pathname =
        output_method == OutputMethod::File ?
            FileUtil::getTempFilename(".txt") :
            FileUtil::getTempFilename(FileFormat::getFileExt(output_file_format).c_str());

    const boost::filesystem::path stderr_pathname =
        FileUtil::getTempFilename(".txt");

    // Ensure temporary files are deleted at end of test.
    FileDeleter output_file_deleter(output_pathname);
    FileDeleter stdout_file_deleter(stdout_pathname);
    FileDeleter stderr_file_deleter(stderr_pathname);

    std::string command_line;

    if (input_method == InputMethod::File) {
        command_line = "./audiowaveform";
        command_line += " -i " + input_pathname.string();
    }
    else if (input_method == InputMethod::Pipe) {
        command_line = "cat ";
        command_line += input_pathname.string();
        command_line += " | ./audiowaveform";
        command_line += " --input-format ";
        command_line += FileFormat::toString(input_file_format);
    }
    else {
        command_line = "./audiowaveform";
        command_line += " --input-format ";
        command_line += FileFormat::toString(input_file_format);
    }

    if (output_method == OutputMethod::File) {
        command_line += " -o ";
        command_line += output_pathname.string();
    }
    else {
        command_line += " --output-format ";
        command_line += FileFormat::toString(output_file_format);
    }

    if (args != nullptr) {
        for (const auto& arg : *args) {
            command_line += " ";
            command_line += arg;
        }
    }

    command_line += " 1>";
    command_line += stdout_pathname.string();
    command_line += " 2>";
    command_line += stderr_pathname.string();

    if (input_method == InputMethod::StdIn) {
        command_line += " <";
        command_line += input_pathname.string();
    }

    const int result = system(command_line.c_str());
    ASSERT_THAT(result, Ne(-1)) << command_line;
    const int exit_status = WEXITSTATUS(result);

    std::vector<uint8_t> output_buffer = FileUtil::readFile(stdout_pathname.string().c_str());
    std::vector<uint8_t> error_buffer  = FileUtil::readFile(stderr_pathname.string().c_str());

    std::string output(output_buffer.begin(), output_buffer.end());
    std::string error(error_buffer.begin(), error_buffer.end());

    if (should_succeed) {
        ASSERT_THAT(exit_status, Eq(0)) << command_line;

        // Check file was created.
        bool exists = boost::filesystem::is_regular_file(output_pathname);

        if (output_method == OutputMethod::File) {
            ASSERT_TRUE(exists) << command_line;
        }
        else {
            ASSERT_FALSE(exists) << command_line;
        }

        if (reference_filename) {
            boost::filesystem::path reference_pathname = "../test/data";
            reference_pathname /= reference_filename;

            const boost::filesystem::path& test_pathname =
                output_method == OutputMethod::File ? output_pathname : stdout_pathname;

            if (output_file_format == FileFormat::Png) {
                compareImageFiles(test_pathname, reference_pathname);
            }
            else {
                compareFiles(test_pathname, reference_pathname);
            }
        }

        // Check no error message was output.
        ASSERT_THAT(error, EndsWith("Done\n")) << command_line;

        if (output_method == OutputMethod::File) {
            // Check nothing was written to standard output.
            ASSERT_THAT(output, StrEq("")) << command_line;
        }
    }
    else {
        ASSERT_THAT(exit_status, Eq(1)) << command_line;

        // Check output file was not created.
        bool exists = boost::filesystem::is_regular_file(output_pathname);
        ASSERT_FALSE(exists) << command_line;

        // Check error message.
        if (error_message != nullptr) {
            ASSERT_THAT(error, EndsWith(error_message)) << command_line;
        }
        else {
            const std::string expected(
                "Can't generate " + FileFormat::toString(output_file_format) +
                " format output from " + FileFormat::toString(input_file_format) +
                " format input\n"
            );

            ASSERT_THAT(error, StrEq(expected)) << command_line;
            ASSERT_THAT(error, StartsWith("Can't generate")) << command_line;
            ASSERT_THAT(error, HasSubstr(FileFormat::toString(input_file_format))) << command_line;
            ASSERT_THAT(error, HasSubstr(FileFormat::toString(output_file_format))) << command_line;
            ASSERT_THAT(error, EndsWith("\n")) << command_line;
        }

        // Check nothing was written to standard output.
        ASSERT_THAT(output, StrEq("")) << command_line;
    }
}

//------------------------------------------------------------------------------

static void runTests(
    const char* input_filename,
    FileFormat::FileFormat input_file_format,
    FileFormat::FileFormat output_file_format,
    const std::vector<const char*>* args,
    bool should_succeed,
    const char* reference_filename = nullptr,
    const char* error_message = nullptr)
{
    runTest(
        InputMethod::File,
        input_filename,
        input_file_format,
        OutputMethod::File,
        output_file_format,
        args,
        should_succeed,
        reference_filename,
        error_message
    );

    runTest(
        InputMethod::File,
        input_filename,
        input_file_format,
        OutputMethod::StdOut,
        output_file_format,
        args,
        should_succeed,
        reference_filename,
        error_message
    );

    runTest(
        InputMethod::StdIn,
        input_filename,
        input_file_format,
        OutputMethod::File,
        output_file_format,
        args,
        should_succeed,
        reference_filename,
        error_message
    );

    runTest(
        InputMethod::StdIn,
        input_filename,
        input_file_format,
        OutputMethod::StdOut,
        output_file_format,
        args,
        should_succeed,
        reference_filename,
        error_message
    );

    // See https://github.com/bbc/audiowaveform/issues/103
    if (input_file_format != FileFormat::Flac &&
        input_file_format != FileFormat::Ogg) {
        runTest(
            InputMethod::Pipe,
            input_filename,
            input_file_format,
            OutputMethod::File,
            output_file_format,
            args,
            should_succeed,
            reference_filename,
            error_message
        );

        runTest(
            InputMethod::Pipe,
            input_filename,
            input_file_format,
            OutputMethod::StdOut,
            output_file_format,
            args,
            should_succeed,
            reference_filename,
            error_message
        );
    }
}

//------------------------------------------------------------------------------
//
// Audio format conversion tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvertMp3ToWavAudio)
{
    runTests("test_file_mono.mp3", FileFormat::Mp3, FileFormat::Wav, nullptr, true, "test_file_mono_converted.wav");
}

//------------------------------------------------------------------------------


TEST_F(OptionHandlerTest, shouldNotConvertWavToMp3Audio)
{
    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Mp3, nullptr, false);
}

//------------------------------------------------------------------------------
//
// Waveform data generation tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromWavAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Dat, &args, true, "test_file_stereo_8bit_64spp_wav.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerate2ChannelBinaryWaveformDataFromWavAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64", "--split-channels" };

    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Dat, &args, true, "test_file_2channel_8bit_64spp_wav.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromFloatingPointWavAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_mono_float32.wav", FileFormat::Wav, FileFormat::Dat, &args, true, "test_file_mono_float32_8bit_64spp.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromMp3Audio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.mp3", FileFormat::Mp3, FileFormat::Dat, &args, true, "test_file_stereo_8bit_64spp_mp3.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromFlacAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.flac", FileFormat::Flac, FileFormat::Dat, &args, true, "test_file_stereo_8bit_64spp_flac.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromOggVorbisAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.oga", FileFormat::Ogg, FileFormat::Dat, &args, true, "test_file_stereo_8bit_64spp_oga.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromOpusAudioIfSupported)
{
    if (FileFormat::isSupported(FileFormat::Opus)) {
        std::vector<const char*> args{ "-b", "8", "-z", "64" };

        runTests("test_file_stereo.opus", FileFormat::Opus, FileFormat::Dat, &args, true, "test_file_stereo_8bit_64spp_opus.dat");
    }
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotGenerateBinaryWaveformDataFromOpusAudioIfNotSupported)
{
    if (!FileFormat::isSupported(FileFormat::Opus)) {
        std::vector<const char*> args{ "-b", "8", "-z", "64" };

        runTests("test_file_stereo.opus", FileFormat::Opus, FileFormat::Dat, &args, false);
    }
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromWavAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Json, &args, true, "test_file_stereo_8bit_64spp_wav.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromMp3Audio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.mp3", FileFormat::Mp3, FileFormat::Json, &args, true, "test_file_stereo_8bit_64spp_mp3.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromFlacAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.flac", FileFormat::Flac, FileFormat::Json, &args, true, "test_file_stereo_8bit_64spp_flac.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromOggVorbisAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };

    runTests("test_file_stereo.oga", FileFormat::Ogg, FileFormat::Json, &args, true, "test_file_stereo_8bit_64spp_oga.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromOpusAudioIfSupported)
{
    if (FileFormat::isSupported(FileFormat::Opus)) {
        std::vector<const char*> args{ "-b", "8", "-z", "64" };

        runTests("test_file_stereo.opus", FileFormat::Opus, FileFormat::Json, &args, true, "test_file_stereo_8bit_64spp_opus.json");
    }
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotGenerateJsonWaveformDataFromOpusAudioIfNotSupported)
{
    if (!FileFormat::isSupported(FileFormat::Opus)) {
        std::vector<const char*> args{ "-b", "8", "-z", "64" };

        runTests("test_file_stereo.opus", FileFormat::Opus, FileFormat::Json, &args, false);
    }
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataWithAutoAmplitudeScale)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64", "--amplitude-scale", "auto" };

    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Dat, &args, true, "test_file_stereo_8bit_64spp_wav_auto_scale.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataWithAutoAmplitudeScale)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64", "--amplitude-scale", "auto" };

    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Json, &args, true, "test_file_stereo_8bit_64spp_wav_auto_scale.json");
}

//------------------------------------------------------------------------------
//
// Waveform data format conversion tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvertBinaryWaveformDataToJson)
{
    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Json, nullptr, true, "test_file_stereo_8bit_64spp_wav.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvert2ChannelBinaryWaveformDataToJson)
{
    runTests("07023003_8bit_64spp_2channel.dat", FileFormat::Dat, FileFormat::Json, nullptr, true, "07023003_8bit_64spp_2channel.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvertBinaryWaveformDataToText)
{
    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Txt, nullptr, true, "test_file_stereo_8bit_64spp_wav.txt");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvert2ChannelBinaryWaveformDataToText)
{
    runTests("07023003_8bit_64spp_2channel.dat", FileFormat::Dat, FileFormat::Txt, nullptr, true, "07023003_8bit_64spp_2channel.txt");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertJsonWaveformDataToBinary)
{
    runTests("test_file_stereo_8bit_64spp_mp3.json", FileFormat::Json, FileFormat::Dat, nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertJsonWaveformDataToText)
{
    runTests("test_file_stereo_8bit_64spp_mp3.json", FileFormat::Json, FileFormat::Txt, nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertTextWaveformDataToBinary)
{
    runTests("test_file_stereo_8bit_64spp_wav.txt", FileFormat::Txt, FileFormat::Dat, nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertTextWaveformDataToJson)
{
    runTests("test_file_stereo_8bit_64spp_wav.txt", FileFormat::Txt, FileFormat::Json, nullptr, false);
}

//------------------------------------------------------------------------------
//
// Waveform image rendering tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderSingleChannelWaveformImageWithCorrectAmplitudeScale)
{
    std::vector<const char*> args{ "-z", "64" };

    runTests("test_file_image_amplitude_scale_1channel.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_image_amplitude_scale_1channel.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderTwoChannelWaveformImageWithCorrectAmplitudeScale)
{
    std::vector<const char*> args{ "-z", "64" };

    runTests("test_file_image_amplitude_scale_2channel.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_image_amplitude_scale_2channel.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromBinaryWaveformData)
{
    std::vector<const char*> args{ "-z", "128" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromFloatingPointWavAudio)
{
    std::vector<const char*> args{ "-z", "128" };

    runTests("test_file_mono_float32.wav", FileFormat::Wav, FileFormat::Png, &args, true, "test_file_mono_float32_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromWavAudio)
{
    std::vector<const char*> args{ "-z", "128" };

    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, true, "test_file_stereo_wav_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromMp3Audio)
{
    std::vector<const char*> args{ "-z", "128" };

    runTests("test_file_stereo.mp3", FileFormat::Mp3, FileFormat::Png, &args, true, "test_file_stereo_mp3_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromFlacAudio)
{
    std::vector<const char*> args{ "-z", "128" };

    runTests("test_file_stereo.flac", FileFormat::Flac, FileFormat::Png, &args, true, "test_file_stereo_flac_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromOggVorbisAudio)
{
    std::vector<const char*> args{ "-z", "128" };

    runTests("test_file_stereo.oga", FileFormat::Ogg, FileFormat::Png, &args, true, "test_file_stereo_oga_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromOpusAudioIfSupported)
{
    if (FileFormat::isSupported(FileFormat::Opus)) {
        std::vector<const char*> args{ "-z", "128" };

        runTests("test_file_stereo.opus", FileFormat::Opus, FileFormat::Png, &args, true, "test_file_stereo_opus_128spp.png");
    }
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotRenderWaveformImageFromOpusAudioIfNotSupported)
{
    if (!FileFormat::isSupported(FileFormat::Opus)) {
        std::vector<const char*> args{ "-z", "128" };

        runTests("test_file_stereo.opus", FileFormat::Opus, FileFormat::Png, &args, false);
    }
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithColorScheme)
{
    std::vector<const char*> args{ "-z", "128", "--colors", "audition" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_audition.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithSpecifiedColors)
{
    std::vector<const char*> args{
        "-z", "128",
        "--border-color", "ff0000",
        "--background-color", "00ff0080",
        "--waveform-color", "0000ff",
        "--axis-label-color", "ffffff",
    };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_colors.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithNoAxisLabels)
{
    std::vector<const char*> args{ "-z", "128", "--no-axis-labels" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_no_axis_labels.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithFixedAmplitudeScale)
{
    std::vector<const char*> args{ "-z", "128", "--amplitude-scale", "1.5" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_scale_1.5.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithAutoAmplitudeScale)
{
    std::vector<const char*> args{ "-z", "128", "--amplitude-scale", "auto" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_scale_auto.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformFitToImageWidthFromMp3)
{
    std::vector<const char*> args{ "-z", "auto", "-w", "500", "-h", "150" };

    runTests("test_file_stereo.mp3", FileFormat::Mp3, FileFormat::Png, &args, true, "test_file_stereo_mp3_500.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformFitToImageWidthFromWav)
{
    std::vector<const char*> args{ "-z", "auto", "-w", "500", "-h", "150" };

    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, true, "test_file_stereo_wav_500.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithSplitChannels)
{
    std::vector<const char*> args{ "-z", "128", "--split-channels" };

    runTests("test_file_2channel_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_2channel_8bit_64spp_wav.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithStartTimeOffset)
{
    std::vector<const char*> args{ "-z", "128", "--start", "0.5" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_offset.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithSquareBars)
{
    std::vector<const char*> args{ "-z", "128", "--waveform-style", "bars" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_square_bars.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithRoundedBars)
{
    std::vector<const char*> args{ "-z", "128", "--waveform-style", "bars", "--bar-style", "rounded" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_rounded_bars.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformBarsWithStartTimeOffset)
{
    std::vector<const char*> args{ "-z", "128", "--waveform-style", "bars", "--start", "0.5" };

    runTests("test_file_stereo_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_stereo_dat_128spp_square_bars_offset.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformBarsWithSplitChannels)
{
    std::vector<const char*> args{ "-z", "128", "--waveform-style", "bars", "--split-channels" };

    runTests("test_file_2channel_8bit_64spp_wav.dat", FileFormat::Dat, FileFormat::Png, &args, true, "test_file_2channel_8bit_64spp_wav_bars.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotRenderWaveformImageFromJsonWaveformData)
{
    runTests("test_file_stereo_8bit_64spp_wav.json", FileFormat::Json, FileFormat::Png, nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotRenderWaveformImageFromTextWaveformData)
{
    runTests("test_file_stereo_8bit_64spp_wav.txt", FileFormat::Txt, FileFormat::Png, nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfZoomIsZeroWithWavAudio)
{
    std::vector<const char*> args{ "-z", "0" };
    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, false, nullptr, "Invalid zoom: minimum 2\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfZoomIsZeroWithMp3Audio)
{
    std::vector<const char*> args{ "-z", "0" };
    runTests("test_file_stereo.mp3", FileFormat::Mp3, FileFormat::Png, &args, false, nullptr, "Invalid zoom: minimum 2\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfPixelsPerSecondIsZero)
{
    std::vector<const char*> args{ "--pixels-per-second", "0" };
    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, false, nullptr, "Invalid pixels per second: must be greater than zero\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfPixelsPerSecondIsNegative)
{
    std::vector<const char*> args{ "--pixels-per-second", "-1" };
    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, false, nullptr, "Invalid pixels per second: must be greater than zero\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfUnknownColorScheme)
{
    std::vector<const char*> args{ "--colors", "test" };
    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, false, nullptr, "Unknown color scheme: test\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfUnknownWaveformStyle)
{
    std::vector<const char*> args{ "--waveform-style", "test" };
    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, false, nullptr, "Unknown waveform style: test\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfUnknownWaveformBarStyle)
{
    std::vector<const char*> args{ "--waveform-style", "bars", "--bar-style", "test" };
    runTests("test_file_stereo.wav", FileFormat::Wav, FileFormat::Png, &args, false, nullptr, "Unknown waveform bar style: test\n");
}

//------------------------------------------------------------------------------
