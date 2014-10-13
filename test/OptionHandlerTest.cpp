//------------------------------------------------------------------------------
//
// Copyright 2014 BBC Research and Development
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

//------------------------------------------------------------------------------

using testing::StartsWith;
using testing::EndsWith;
using testing::Eq;
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
static void compare(const std::vector<T>& a, const std::vector<T>& b)
{
    ASSERT_THAT(a.size(), Eq(b.size()));

    const auto size = a.size();

    for (std::size_t i = 0; i != size; ++i) {
        ASSERT_THAT(a[i], Eq(b[i]));
    }
}

//------------------------------------------------------------------------------

static void compareFiles(const boost::filesystem::path& a, const boost::filesystem::path& b)
{
    std::vector<uint8_t> file_a_data = FileUtil::readFile(a);
    std::vector<uint8_t> file_b_data = FileUtil::readFile(b);

    compare(file_a_data, file_b_data);
}

//------------------------------------------------------------------------------

static void runTest(
    const char* input_filename,
    const char* output_file_ext,
    const std::vector<const char*>* args,
    bool should_succeed,
    const char* reference_filename = nullptr)
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
    ASSERT_EQ(should_succeed, success);

    if (should_succeed) {
        // Check file was created.
        bool exists = boost::filesystem::is_regular_file(output_pathname);
        ASSERT_TRUE(exists);

        if (reference_filename) {
            boost::filesystem::path reference_pathname = "../test/data";
            reference_pathname /= reference_filename;

            compareFiles(output_pathname, reference_pathname);
        }

        ASSERT_FALSE(output.str().empty());

        // Check no error message was output.
        ASSERT_TRUE(error.str().empty());
    }
    else {
        // Check output file was not created.
        bool exists = boost::filesystem::is_regular_file(output_pathname);
        ASSERT_FALSE(exists);

        ASSERT_TRUE(output.str().empty());

        // Check error message.
        const std::string str = error.str();
        ASSERT_THAT(str, StartsWith("Can't generate"));
        ASSERT_THAT(str, EndsWith("\n"));
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
