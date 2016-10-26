//------------------------------------------------------------------------------
//
// Copyright 2014, 2015 BBC Research and Development
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
#include "Options.h"
#include "Array.h"
#include "util/FileDeleter.h"
#include "util/FileUtil.h"
#include "util/Streams.h"

#include "gmock/gmock.h"

#include <gd.h>
#include <string.h>

//------------------------------------------------------------------------------

using testing::StartsWith;
using testing::EndsWith;
using testing::Eq;
using testing::NotNull;
using testing::StrEq;
using testing::Test;

//------------------------------------------------------------------------------

class OptionHandlerTest : public Test
{
    public:
        OptionHandlerTest()
        {
        }

    protected:
        virtual void SetUp()
        {
            output.str(std::string());
            error.str(std::string());
        }

        virtual void TearDown()
        {
        }
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

static void compareImageFiles(
    const boost::filesystem::path& test_filename,
    const boost::filesystem::path& ref_filename)
{
    gdImagePtr test_image = openImageFile(test_filename);
    ASSERT_THAT(test_image, NotNull());

    gdImagePtr ref_image = openImageFile(ref_filename);
    ASSERT_THAT(ref_image, NotNull());

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

    gdImageDestroy(test_image);
    gdImageDestroy(ref_image);
}

//------------------------------------------------------------------------------

static void runTest(
    const char* input_filename,
    const char* output_file_ext,
    const std::vector<const char*>* args,
    bool should_succeed,
    const char* reference_filename = nullptr,
    const char* error_message = nullptr)
{
    boost::filesystem::path input_pathname = "../test/data";
    input_pathname /= input_filename;

    const boost::filesystem::path output_pathname =
        FileUtil::getTempFilename(output_file_ext);

    // Ensure temporary file is deleted at end of test.
    FileDeleter deleter(output_pathname);

    std::vector<const char*> argv{
        "appname",
        "-i", input_pathname.c_str(),
        "-o", output_pathname.c_str()
    };

    if (args != nullptr) {
        for (const auto& i : *args) {
            argv.push_back(i);
        }
    }

    Options options;

    bool success = options.parseCommandLine(static_cast<int>(argv.size()), &argv[0]);
    ASSERT_TRUE(success);

    OptionHandler option_handler;

    success = option_handler.run(options);
    ASSERT_THAT(success, Eq(should_succeed));

    if (should_succeed) {
        // Check file was created.
        bool exists = boost::filesystem::is_regular_file(output_pathname);
        ASSERT_TRUE(exists);

        if (reference_filename) {
            boost::filesystem::path reference_pathname = "../test/data";
            reference_pathname /= reference_filename;

            if (strcmp(output_file_ext, ".png") == 0) {
                compareImageFiles(output_pathname, reference_pathname);
            }
            else {
                compareFiles(output_pathname, reference_pathname);
            }
        }

        ASSERT_FALSE(output.str().empty());

        // Check no error message was output.
        ASSERT_TRUE(error.str().empty());
    }
    else {
        // Check output file was not created.
        bool exists = boost::filesystem::is_regular_file(output_pathname);
        ASSERT_FALSE(exists);

        // Check error message.
        const std::string str = error.str();

        if (error_message != nullptr) {
            ASSERT_THAT(str, StrEq(error_message));
        }
        else {
            ASSERT_THAT(str, StartsWith("Can't generate"));
            ASSERT_THAT(str, EndsWith("\n"));

            ASSERT_TRUE(output.str().empty());
            ASSERT_THAT(output.str(), StrEq(""));
        }
    }
}

//------------------------------------------------------------------------------
//
// Audio format conversion tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvertMp3ToWavAudio)
{
    runTest("test_file_mono.mp3", ".wav", nullptr, true, "test_file_mono_converted.wav");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertWavToMp3Audio)
{
    runTest("test_file_stereo.wav", ".mp3", nullptr, false);
}

//------------------------------------------------------------------------------
//
// Waveform data generation tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromWavAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };
    runTest("test_file_stereo.wav", ".dat", &args, true, "test_file_stereo_8bit_64spp.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromFloatingPointWavAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };
    runTest("test_file_mono_float32.wav", ".dat", &args, true, "test_file_mono_float32_8bit_64spp.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromMp3Audio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };
    runTest("test_file_stereo.mp3", ".dat", &args, true, "test_file_stereo_8bit_64spp.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateBinaryWaveformDataFromFlacAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };
    runTest("test_file_stereo.flac", ".dat", &args, true, "test_file_stereo_8bit_64spp.dat");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromWavAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };
    runTest("test_file_stereo.wav", ".json", &args, true, "test_file_stereo_8bit_64spp.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromMp3Audio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };
    runTest("test_file_stereo.mp3", ".json", &args, true, "test_file_stereo_8bit_64spp_mp3.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldGenerateJsonWaveformDataFromFlacAudio)
{
    std::vector<const char*> args{ "-b", "8", "-z", "64" };
    runTest("test_file_stereo.flac", ".json", &args, true, "test_file_stereo_8bit_64spp.json");
}

//------------------------------------------------------------------------------
//
// Waveform data format conversion tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvertBinaryWaveformDataToJson)
{
    runTest("test_file_stereo_8bit_64spp.dat", ".json", nullptr, true, "test_file_stereo_8bit_64spp.json");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldConvertBinaryWaveformDataToText)
{
    runTest("test_file_stereo_8bit_64spp.dat", ".txt", nullptr, true, "test_file_stereo_8bit_64spp.txt");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertJsonWaveformDataToBinary)
{
    runTest("test_file_stereo_8bit_64spp.json", ".dat", nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertJsonWaveformDataToText)
{
    runTest("test_file_stereo_8bit_64spp.json", ".txt", nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertTextWaveformDataToBinary)
{
    runTest("test_file_stereo_8bit_64spp.txt", ".dat", nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotConvertTextWaveformDataToJson)
{
    runTest("test_file_stereo_8bit_64spp.txt", ".json", nullptr, false);
}

//------------------------------------------------------------------------------
//
// Waveform image rendering tests
//
//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromBinaryWaveformData)
{
    std::vector<const char*> args{ "-z", "128" };
    runTest("test_file_stereo_8bit_64spp.dat", ".png", &args, true, "test_file_stereo_dat_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromFloatingPointWavAudio)
{
    std::vector<const char*> args{ "-z", "128" };
    runTest("test_file_mono_float32.wav", ".png", &args, true, "test_file_mono_float32_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromWavAudio)
{
    std::vector<const char*> args{ "-z", "128" };
    runTest("test_file_stereo.wav", ".png", &args, true, "test_file_stereo_wav_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromMp3Audio)
{
    std::vector<const char*> args{ "-z", "128" };
    runTest("test_file_stereo.mp3", ".png", &args, true, "test_file_stereo_wav_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformImageFromFlacAudio)
{
    std::vector<const char*> args{ "-z", "128" };
    runTest("test_file_stereo.flac", ".png", &args, true, "test_file_stereo_wav_128spp.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithColorScheme)
{
    std::vector<const char*> args{ "-z", "128", "--colors", "audition" };

    runTest("test_file_stereo_8bit_64spp.dat", ".png", &args, true, "test_file_stereo_dat_128spp_audition.png");
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

    runTest("test_file_stereo_8bit_64spp.dat", ".png", &args, true, "test_file_stereo_dat_128spp_colors.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithNoAxisLabels)
{
    std::vector<const char*> args{ "-z", "128", "--no-axis-labels" };

    runTest("test_file_stereo_8bit_64spp.dat", ".png", &args, true, "test_file_stereo_dat_128spp_no_axis_labels.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithFixedAmplitudeScale)
{
    std::vector<const char*> args{ "-z", "128", "--amplitude-scale", "1.5" };

    runTest("test_file_stereo_8bit_64spp.dat", ".png", &args, true, "test_file_stereo_dat_128spp_scale_1.5.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldRenderWaveformWithAutoAmplitudeScale)
{
    std::vector<const char*> args{ "-z", "128", "--amplitude-scale", "auto" };

    runTest("test_file_stereo_8bit_64spp.dat", ".png", &args, true, "test_file_stereo_dat_128spp_scale_auto.png");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotRenderWaveformImageFromJsonWaveformData)
{
    runTest("test_file_stereo.json", ".png", nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldNotRenderWaveformImageFromTextWaveformData)
{
    runTest("test_file_stereo.txt", ".png", nullptr, false);
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfZoomIsZero)
{
    std::vector<const char*> args{ "-z", "0" };
    runTest("test_file_stereo.wav", ".png", &args, false, nullptr, "Invalid zoom: minimum 2\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfPixelsPerSecondIsZero)
{
    std::vector<const char*> args{ "--pixels-per-second", "0" };
    runTest("test_file_stereo.wav", ".png", &args, false, nullptr, "Invalid pixels per second: must be greater than zero\n");
}

//------------------------------------------------------------------------------

TEST_F(OptionHandlerTest, shouldFailIfPixelsPerSecondIsNegative)
{
    std::vector<const char*> args{ "--pixels-per-second", "-1" };
    runTest("test_file_stereo.wav", ".png", &args, false, nullptr, "Invalid pixels per second: must be greater than zero\n");
}

//------------------------------------------------------------------------------
