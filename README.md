# Audio Waveform Image Generator

[![Build Status](https://travis-ci.org/bbc/audiowaveform.svg?branch=master)](https://travis-ci.org/bbc/audiowaveform)

**audiowaveform** is a C++ command-line application that generates waveform data
from either MP3, WAV, or FLAC format audio files. Waveform data can be used to
produce a visual rendering of the audio, similar in appearance to audio editing
applications.

Waveform data files are saved in either binary format (.dat) or JSON (.json).
Given an input waveform data file, **audiowaveform** can also render the audio
waveform as a PNG image at a given time offset and zoom level.

The waveform data is produced from an input stereo audio signal by first
combining the left and right channels to produce a mono signal. The next stage
is to compute the minimum and maximum sample values over groups of *N* input
samples (where *N* is controlled by the `--zoom` command-line option), such that
each *N* input samples produces one pair of minimum and maxmimum points in the
output.

## Installation

### Ubuntu

Binary packages are available on Ubuntu Launchpad [here](https://launchpad.net/~chris-needham/+archive/ubuntu/ppa).

    $ sudo add-apt-repository ppa:chris-needham/ppa
    $ sudo apt-get update
    $ sudo apt-get install audiowaveform

### Mac OSX via Homebrew

To install `audiowaveform` using Homebrew:

    $ brew tap bbc/audiowaveform
    $ brew install audiowaveform

### Windows

Windows binaries are not currently available. Please follow [this issue](https://github.com/bbc/audiowaveform/issues/23) for details.

## Building from source

**audiowaveform** requires [cmake](http:///www.cmake.org) 2.8.7 or later, g++
4.6.3 or later, and [Boost](http://www.boost.org) 1.46.0 or later.

The software has been developed on Ubuntu 12.04 and Fedora 18. Due to compiler
and library version requirements, the software may not build on earlier
operating system releases.

### Install package dependencies

#### Fedora

libmad is available from the RPM Fusion **free** repository. Before running the
following `yum` command you should follow the instructions
[here](http://rpmfusion.org/Configuration) to add this repository, if you have
not already done so.

    $ sudo yum install git cmake libmad-devel libsndfile-devel \
      gd-devel boost-devel

#### Ubuntu

    $ sudo apt-get install git-core make cmake gcc g++ libmad0-dev libsndfile1-dev \
      libgd2-xpm-dev libboost-filesystem-dev libboost-program-options-dev \
      libboost-regex-dev

#### Mac OSX

Install [XCode](https://developer.apple.com/xcode/) and
[Homebrew](http://mxcl.github.io/homebrew/), then:

    $ brew install cmake libmad libsndfile gd
    $ brew install boost --with-c++11

### Obtain the source code

    $ git clone https://github.com/bbc/audiowaveform.git
    $ cd audiowaveform

### Install Google Test test framework

**audiowaveform** uses [Google Test](https://github.com/google/googletest)
for unit testing.
Following [this advice](https://github.com/google/googletest/blob/master/googletest/docs/FAQ.md#why-is-it-not-recommended-to-install-a-pre-compiled-copy-of-google-test-for-example-into-usrlocal)
in the Google Test FAQ, download the source and unzip:

    $ wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    $ tar xzf release-1.8.0.tar.gz
    $ ln -s googletest-release-1.8.0/googletest googletest
    $ ln -s googletest-release-1.8.0/googlemock googlemock

### Build

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

The default build type is Release. To build in Debug mode add
`-D CMAKE_BUILD_TYPE=Debug` to the `cmake` command above. If you don't want to
compile the unit tests add `-D ENABLE_TESTS=0`.

To compile with clang instead of g++:

    $ cmake -D CMAKE_C_COMPILER=/usr/local/bin/clang -D CMAKE_CXX_COMPILER=/usr/local/bin/clang++ ..

### Test

    $ make test

To see detailed test output:

    $ ./audiowaveform_tests

### Install

    $ sudo make install

By default this installs the `audiowaveform` program in `/usr/local/bin`, and
man pages in `/usr/local/share/man`. To change these locations, add a `-D
CMAKE_INSTALL_PREFIX=...` option when invoking `cmake` above.

### Run

    $ audiowaveform --help

## Command line options

**audiowaveform** accepts the following command-line options:

| Short           | Long                           | Description                                                                                                   |
| --------------- | ------------------------------ | ------------------------------------------------------------------------------------------------------------- |
|                 | `--help`                       | Show help message                                                                                             |
| `-v`            | `--version`                    | Show version information                                                                                      |
| `-i <filename>` | `--input-filename <filename>`  | Input mono or stereo audio (.wav or .mp3) or waveform data (.dat) file name                                   |
| `-o <filename>` | `--output-filename <filename>` | Output waveform data (.dat or .json), audio (.wav), or PNG image (.png) file name                             |
| `-z <level>`    | `--zoom <zoom>`                | Zoom level (samples per pixel), default: 256. Not valid if `--end` or `--pixels-per-second` is also specified |
|                 | `--pixels-per-second <zoom>`   | Zoom level (pixels per second), default: 100. Not valid if `--end` or `--zoom` is also specified              |
| `-b <bits>`     | `--bits <bits>`                | Number of bits resolution when creating a waveform data file (either 8 or 16), default: 16                    |
| `-s <seconds>`  | `--start <seconds>`            | Start time (seconds), default: 0                                                                              |
| `-e <seconds>`  | `--end <seconds>`              | End time (seconds). Not valid if `--zoom` is also specified                                                   |
| `-w <width>`    | `--width <width>`              | Width of output image (pixels), default: 800                                                                  |
| `-h <height>`   | `--height <height>`            | Height of output image (pixels), default: 250                                                                 |
| `-c <scheme>`   | `--colors <scheme>`            | Color scheme of output image (either 'audition' or 'audacity'), default: audacity                             |
|                 | `--border-color <color>`       | Border color (in rrggbb\[aa\] hex format), default: set by `--colors` option                                  |
|                 | `--background-color <color>`   | Background color (in rrggbb\[aa\] hex format), default: set by `--colors` option                              |
|                 | `--waveform-color <color>`     | Waveform color (in rrggbb\[aa\] hex format), default: set by `--colors` option                                |
|                 | `--axis-label-color <color>`   | Axis label color (in rrggbb\[aa\] hex format), default: set by `--colors` option                              |
|                 | `--no-axis-labels`             | Render PNG images without axis labels                                                                         |
|                 | `--with-axis-labels`           | Render PNG images with axis labels (default)                                                                  |
|                 | `--amplitude-scale <scale>`    | Amplitude scale (number or `auto`), default: 1                                                                |
|                 | `--compression <level>`        | PNG compression level: 0 (none) to 9 (best), or -1 (default)                                                  |

### Usage

In general, you should use **audiowaveform** to create waveform data files
(.dat) from input MP3 or WAV audio files, then create waveform images from the
waveform data files.

For example, to create a waveform data file from an MP3 file, at 256 samples
per point with 8-bit resolution:

    $ audiowaveform -i test.mp3 -o test.dat -z 256 -b 8

Then, to create a PNG image of a waveform, either specify the zoom level, in
samples per pixel, or the time region to render.

The following command creates a 1000x200 pixel PNG image from a waveform data
file, at 50 pixels per second, starting at 5.0 seconds from the start of the
audio:

    $ audiowaveform -i test.dat -o test.png --pixels-per-second 50 -s 5.0 -w 1000 -h 200

This command creates a 1000x200 pixel PNG image from a waveform data file,
showing the region from 45.0 seconds to 60.0 seconds from the start of the
audio:

    $ audiowaveform -i test.dat -o test.png -s 45.0 -e 60.0 -w 1000 -h 200

Note that it is not possible to set a zoom level less than that used to create
the original waveform data file.

It is also possible to create PNG images directly from either MP3 or WAV
files, although if you want to render multiple images from the same audio
file, it's generally preferable to first create a waveform data (.dat) file,
and create the images from that, as decoding long MP3 files can take
significant time.

The following command creates a 1000x200 PNG image directly from a WAV file,
at 300 samples per pixel, starting at 60.0 seconds from the start of the
audio:

    $ audiowaveform -i test.wav -o test.png -z 300 -s 60.0 -w 1000 -h 200

The following command converts a waveform data file (.dat) to JSON format:

    $ audiowaveform -i test.dat -o test.json

In addition, **audiowaveform** can also be used to convert MP3 to WAV format
audio:

    $ audiowaveform -i test.mp3 -o test.wav

## Credits

This program contains code from the following open-source projects, used under
the terms of these projects' respective licenses:

* [Audacity](http://audacity.sourceforge.net/)
* [madlld](http://www.bsd-dk.dk/~elrond/audio/madlld/)

## License

See COPYING for details.

## Contributing

If you have a feature request or want to report a bug, we'd be happy to hear
from you. Please either
[raise an issue](https://github.com/bbcrd/audiowaveform/issues), or fork the
project and send us a pull request.

## Authors

This software was written by [Chris Needham](https://github.com/chrisn),
chris.needham at bbc.co.uk.

## Copyright

Copyright 2013-2016 British Broadcasting Corporation
