# Audio Waveform Image Generator

![Build Status](https://github.com/bbc/audiowaveform/workflows/CMake/badge.svg?branch=master)

**audiowaveform** is a C++ command-line application that generates waveform data
from either MP3, WAV, FLAC, Ogg Vorbis, or Opus format audio files. Waveform data can
be used to produce a visual rendering of the audio, similar in appearance to
audio editing applications.

![Example Waveform](/doc/example.png "Example Waveform")

Waveform data files are saved in either binary format (.dat) or JSON (.json).
Given an input waveform data file, **audiowaveform** can also render the audio
waveform as a PNG image at a given time offset and zoom level.

The waveform data is produced from an input audio signal by first combining the
input channels to produce a mono signal. The next stage is to compute the
minimum and maximum sample values over groups of *N* input samples (where *N* is
controlled by the `--zoom` command-line option), such that each *N* input
samples produces one pair of minimum and maximum points in the output.

# Contents

- [Installation](#installation)
- [Building from source](#building-from-source)
- [Usage](#usage)
- [Data Formats](#data-formats)
- [Credits](#credits)
- [License](#license)
- [Contributing](#contributing)
- [Authors](#authors)
- [Copyright](#copyright)

## Installation

### Ubuntu

Binary packages are available on Ubuntu Launchpad [here](https://launchpad.net/~chris-needham/+archive/ubuntu/ppa).

    sudo add-apt-repository ppa:chris-needham/ppa
    sudo apt-get update
    sudo apt-get install audiowaveform

### CentOS

Binary packages are available on the [Releases](https://github.com/bbc/audiowaveform/releases) page.

Download the correct RPM for your CentOS version and use these commands to install the RPM package,
together with all required dependencies. Replace the version with the latest release version.

    sudo yum install -y epel-release
    sudo yum localinstall audiowaveform-1.5.1-1.el8.x86_64.rpm

### Arch Linux

There is an [`audiowaveform`](https://aur.archlinux.org/packages/audiowaveform) package available in the AUR.

### Mac OSX

You can install `audiowaveform` using Homebrew:

    brew tap bbc/audiowaveform
    brew install audiowaveform

### Windows

Windows binaries are available on the
[Releases](http://github.com/bbc/audiowaveform/releases) page,
and are built using
[compile-static-audiowaveform](https://github.com/chrisn/compile-static-audiowaveform).

### Amazon Linux

A binary package for Amazon Linux 2 is available on the [Releases](https://github.com/bbc/audiowaveform/releases) page.

Use these commands to install the RPM package, together with all required
dependencies. Replace the version with the latest release version.

    sudo amazon-linux-extras install epel
    sudo yum install \
      https://github.com/bbc/audiowaveform/releases/download/1.5.1/audiowaveform-1.5.1-1.amzn2.x86_64.rpm

### Docker

A Docker image based on Alpine Linux is available [here](https://hub.docker.com/r/realies/audiowaveform),
thanks to [@realies](https://github.com/realies).

Example usage:

    docker pull realies/audiowaveform
    alias awf='docker run --rm -v `pwd`:/tmp -w /tmp realies/audiowaveform'
    awf -i input.wav -o output.png

## Building from source

**audiowaveform** requires [cmake](http:///www.cmake.org) 2.8.7 or later, g++
4.6.3 or later, and [Boost](http://www.boost.org) 1.46.0 or later.

The software has been developed on Ubuntu 12.04 and Fedora 18. Due to compiler
and library version requirements, the software may not build on earlier
operating system releases.

### Install package dependencies

#### Fedora

    sudo dnf install git make cmake gcc-c++ libmad-devel \
      libid3tag-devel libsndfile-devel gd-devel boost-devel

#### CentOS 7

Most packages needed to build audiowaveform are already present in CentOS 7,
except `libmad`/`libmad-devel`, which must be taken from the [EPEL](https://fedoraproject.org/wiki/EPEL) repository.

Install the EPEL repository and the `libmad-devel` package:

    sudo yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
    sudo yum install -y --enablerepo=epel libmad-devel

And then install the other build dependencies (other than libmad-devel):

    sudo yum install -y redhat-lsb-core rpm-build wget \
      git make cmake gcc-c++ libid3tag-devel libsndfile-devel gd-devel boost-devel

#### Ubuntu

    sudo apt-get install git make cmake gcc g++ libmad0-dev \
      libid3tag0-dev libsndfile1-dev libgd-dev libboost-filesystem-dev \
      libboost-program-options-dev \
      libboost-regex-dev

Note: for Ubuntu 12.04, replace libgd-dev with libgd2-xpm-dev.

#### Alpine

    apk add git make cmake gcc g++ libmad-dev \
      libid3tag-dev libsndfile-dev gd-dev boost-dev \
      libgd libpng-dev zlib-dev

Note: for a static build you will need to include the following dependencies

    apk add zlib-static libpng-static boost-static

A statically linkable build of FLAC is also required. This is not available in
Alpine so you must compile it yourself.

    apk add autoconf automake libtool gettext
    wget https://github.com/xiph/flac/archive/1.3.3.tar.gz
    tar xzf 1.3.3.tar.gz
    cd flac-1.3.3
    ./autogen.sh
    ./configure --enable-shared=no
    make
    make install

#### Arch

    sudo pacman -S base-devel boost-libs gd \
      libid3tag libmad libsndfile boost cmake git

#### SUSE

    zypper install git cmake gcc-c++ libmad-devel \
      libid3tag-devel libsndfile-devel gd-devel \
      libboost_filesystem1_67_0-devel \
      libboost_program_options1_67_0-devel \
      libboost_regex1_67_0-devel

Note: replace 1_67_0 with the boost version actually available.

#### Mac OSX

Install [XCode](https://developer.apple.com/xcode/) and
[Homebrew](http://mxcl.github.io/homebrew/), then:

    brew install cmake libmad libid3tag libsndfile gd
    brew install boost --with-c++11

### Obtain the source code

    git clone git@github.com:bbc/audiowaveform.git
    cd audiowaveform

### Install Google Test test framework

**audiowaveform** uses [Google Test](https://github.com/google/googletest)
for unit testing.
Following [this advice](https://github.com/google/googletest/blob/master/googletest/README.md#incorporating-into-an-existing-cmake-project)
in the Google Test FAQ, download the source and unzip:

    wget https://github.com/google/googletest/archive/release-1.12.1.tar.gz
    tar xzf release-1.12.1.tar.gz
    ln -s googletest-release-1.12.1 googletest

### Build

    mkdir build
    cd build
    cmake ..
    make

The default build type is Release. To build in Debug mode add
`-D CMAKE_BUILD_TYPE=Debug` to the `cmake` command above:

    cmake -D CMAKE_BUILD_TYPE=Debug ..

If you don't want to compile the unit tests add `-D ENABLE_TESTS=0`:

    cmake -D ENABLE_TESTS=0 ..

To statically link the library dependencies add `-D BUILD_STATIC=1`, for example:

    cmake -D BUILD_STATIC=1 ..

To compile with clang instead of g++:

    cmake -D CMAKE_C_COMPILER=/usr/local/bin/clang -D CMAKE_CXX_COMPILER=/usr/local/bin/clang++ ..

### Test

    make test

To see detailed test output:

    ./audiowaveform_tests

### Package

    make package

The `make package` packages audiowaveform in the native package format for the
local OS (`.deb` for Debian-based systems, `.rpm` for Red Hat-based systems).
The packages are output in the current directory.

The built packages can be locally installed (e.g., `rpm -ivh *.rpm`,
`dpkg -i *.rpm`) or installed on another system, as long as the runtime
dependencies of the package are present (`libmad`, `libsndfile`, `libid3tag`,
`gd` and `boost`).

### Install

    sudo make install

By default this installs the `audiowaveform` program in `/usr/local/bin`, and
man pages in `/usr/local/share/man`. To change these locations, add a `-D
CMAKE_INSTALL_PREFIX=...` option when invoking `cmake` above.

### Run

    audiowaveform --help

## Usage

### Command line options

**audiowaveform** accepts the following command-line options:

#### `--help`

Show help message.

#### `--version`, `-v`

Show version information.

#### `--quiet`, `-q`

Disables status messages.

#### `--input-filename`, `-i <filename>`

Input filename, which should be a MP3, WAV, FLAC, Ogg Vorbis, or Opus audio file, or a
binary waveform data file. By default, audiowaveform uses the file
extension to decide how to read the input file (either .mp3, .wav, .flac, .ogg,
.oga, .opus, or .dat, as appropriate), but this can be overridden by the
`--input-format` option. If the `--input-filename` option is `-` or
is omitted, audiowaveform reads from standard input, and the
`--input-format` option must be used to specify the data format.

Note that Opus support requires libsndfile 1.0.29 or later, so may not be
available on all systems.

#### `--output-filename`, `-o <filename>`

Output filename, which may be either a WAV audio file, a binary or JSON format
waveform data file, or a PNG image file. By default, audiowaveform
uses the file extension to decide the kind of output to generate
(either .wav, .dat, .json, or .png, as appropriate), but this can be overridden
by the `--output-format` option. If the `--output-filename` option is
`-` or is omitted, audiowaveform writes to standard output, and the
`--output-format` option must be used to specify the data format.

#### `--input-format <format>`

Input data format, either `wav`, `mp3`, `flac`, `ogg`, `opus`, or `dat`.
This option must be used when reading from standard input. It may also be used to set
the input file format, instead of it being determined from the file extension
from the `--input-filename` option.

#### `--output-format <format>`

Output data format, either `wav`, `dat`, `json`, or `png`. This
option must be used when writing to standard output. It may also be used to set
the output file format, instead of it being determined from the file extension
from the `--output-filename` option.

#### `--zoom`, `-z <zoom>` (default: 256)

When creating a waveform data file or image, specifies the number of input
samples to use to generate each output waveform data point.
Note: this option cannot be used if either the `--pixels-per-second` or
`--end` option is specified. When creating a PNG image file, a value of
`auto` scales the waveform automatically to fit the image width.

#### `--pixels-per-second <zoom>` (default: 100)

When creating a waveform data file or image, specifies the number of output
waveform data points to generate for each second of audio input.
Note: this option cannot be used if either the `--zoom` or `--end`
option is specified.

#### `--bits`, `-b <bits>` (default: 16)

When creating a waveform data file, specifies the number of data bits to use
for output waveform data points. Valid values are either 8 or 16.

#### `--split-channels`

Output files are multi-channel, not combined into a single waveform.

#### `--start`, `-s <start>` (default: 0)

When creating a waveform image, specifies the start time, in seconds.

#### `--end`, `-e <end>` (default: 0)

When creating a waveform image, specifies the end time, in seconds.
Note: this option cannot be used if the `--zoom` option is specified.

#### `--width`, `-w <width>` (default: 800)

When creating a waveform image, specifies the image width.

#### `--height`, `-h <height>` (default: 250)

When creating a waveform image, specifies the image height.

#### `--colors`, `-c <colors>` (default: `audacity`)

When creating a waveform image, specifies the color scheme to use. Valid values
are either `audacity`, which generates a blue waveform on a grey background,
similar to Audacity, or `audition`, which generates a green waveform on a
dark background, similar to Adobe Audition.

#### `--border-color <rrggbb[aa]>`

When creating a waveform image, specifies the border color. If not given,
the default color used is controlled by the `--colors` option.

The color value should include two hexadecimal digits for each of red, green,
and blue (00 to FF), and optional alpha transparency (00 to FF).

#### `--background-color <rrggbb[aa]>`

When creating a waveform image, specifies the background color. If not given,
the default color used is controlled by the `--colors` option.

#### `--waveform-color <rrggbb[aa]>`

When creating a waveform image, specifies the waveform color. If not given,
the default color used is controlled by the `--colors` option.

#### `--axis-label-color <rrggbb[aa]>`

When creating a waveform image, specifies the axis labels color. If not given,
the default color used is controlled by the `--colors` option.

#### `--with-axis-labels`, `--no-axis-labels` (default: `--with-axis-labels`)

When creating a waveform image, specifies whether to render axis labels and
image border.

#### `--amplitude-scale <scale>` (default: 1)

When creating a waveform image or waveform data file, specifies an amplitude
scaling (or vertical zoom) to apply to the waveform. Must be either a number
or `auto`, which scales the waveform to the maximum height.

#### `--waveform-style <style>` (default: `normal`)

When creating a waveform image, specifies how waveform is drawn. Valid values
are either `normal`, which draws a normal waveform, or `bars`, which
draws the waveform as vertical bars.

#### `--bar-width <width>` (default: 8)

When creating a waveform image with the `--waveform-style bars` option,
specifies the width of each bar, in pixels.

#### `--bar-gap <width>` (default: 4)

When creating a waveform image with the `--waveform-style bars` option,
specifies the separation between each bar, in pixels.

#### `--bar-style <style>` (default: `square`)

When creating a waveform image with the `--waveform-style bars` option,
specifies how each bar is drawn. Valid values are either `square`,
which draws the waveform bars with square corners, or `rounded`, which
draws the waveform with rounded corners.

#### `--compression <level>` (default: -1)

When creating a waveform image, specifies the PNG compression level. Must be
either -1 (default compression) or between 0 (fastest) and 9 (best compression).

### Examples

In general, you should use **audiowaveform** to create waveform data files
(.dat) from input MP3 or WAV audio files, then create waveform images from the
waveform data files.

For example, to create a waveform data file from an MP3 file, at 256 samples
per point with 8-bit resolution:

    audiowaveform -i test.mp3 -o test.dat -z 256 -b 8

Then, to create a PNG image of a waveform, either specify the zoom level, in
samples per pixel. Note that it is not possible to set a zoom level less than
that used to create the original waveform data file.

    audiowaveform -i test.dat -o test.png -z 512

The following command creates a 1000x200 pixel PNG image from a waveform data
file, at 50 pixels per second, starting at 5.0 seconds from the start of the
audio:

    audiowaveform -i test.dat -o test.png --pixels-per-second 50 -s 5.0 -w 1000 -h 200

This command creates a 1000x200 pixel PNG image from a waveform data file,
showing the region from 45.0 seconds to 60.0 seconds from the start of the
audio:

    audiowaveform -i test.dat -o test.png -s 45.0 -e 60.0 -w 1000 -h 200

The `--waveform-style` option allows you to produce waveform images drawn
as vertical bars. The `--bar-width`, `--bar-gap`, and `--bar-style` options
customize the immage:

    audiowaveform -i test.mp3 -o test.png -w 1000 -h 200 -z auto \
      --waveform-style bars --bar-width 10 --bar-gap 2 --bar-style round

You can use the `--split-channels` option to create a waveform data file
containing multiple channels, rather than combining all channels into a single
waveform:

    audiowaveform -i test.mp3 -o test.dat -z 256 -b 8 --split-channels

It is also possible to create PNG images directly from either MP3 or WAV
files, although if you want to render multiple images from the same audio
file, it's generally preferable to first create a waveform data (.dat) file,
and create the images from that, as decoding long MP3 files can take
significant time.

The following command creates a 1000x200 PNG image directly from a WAV file,
at 300 samples per pixel, starting at 60.0 seconds from the start of the
audio:

    audiowaveform -i test.wav -o test.png -z 300 -s 60.0 -w 1000 -h 200

If you are using audiowaveform to generate waveform data for use in a web
application, e.g, using [Peaks.js](https://github.com/bbc/peaks.js), you can
choose whether to use binary or JSON format waveform data.

The following command generates waveform data in JSON format:

    audiowaveform -i test.flac -o test.json -z 256 -b 8

The following command converts a waveform data file (.dat) to JSON format:

    audiowaveform -i test.dat -o test.json

In addition, **audiowaveform** can also be used to convert MP3 to WAV format
audio:

    audiowaveform -i test.mp3 -o test.wav

You can use the `--input-format` and `--output-format` options to read from
standard input and write to standard output. For example, the following command
generates a waveform data file by converting a video file using ffmpeg:

    ffmpeg -i test.mp4 -f wav - | audiowaveform --input-format wav --output-format dat -b 8 > test.dat

Note: Piping audio into **audiowaveform** is currently only supported for MP3
and WAV format audio, and not FLAC or Ogg Vorbis.

## Data Formats

You can find details of the waveform data file formats produced by audiowaveform
[here](doc/DataFormat.md).

## Credits

This program contains code from the following open-source projects, used under
the terms of these projects' respective licenses:

* [Audacity](http://audacity.sourceforge.net/)
* [madlld](http://www.bsd-dk.dk/~elrond/audio/madlld/)

## License

See COPYING for details.

## Contributing

If you'd like to contribute to audiowaveform, please take a look at our
[contributor guidelines](CONTRIBUTING.md).

## Authors

This software was written by [Chris Needham](https://github.com/chrisn),
chris.needham at bbc.co.uk.

Thank you to all our [contributors](https://github.com/bbc/audiowaveform/graphs/contributors).

## Copyright

Copyright 2013-2023 British Broadcasting Corporation
