# Audio Waveform Image Generator

**audiowaveform** is a C++ command-line application that generates waveform data
from either MP3 or WAV format audio files. Waveform data can be used to produce
a visual rendering of the audio, similar in appearance to audio editing
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

## Getting started

**audiowaveform** requires [cmake](http:///www.cmake.org) 2.8.7 or later, g++ 4.6.3 or later, and [Boost](http://www.boost.org) 1.46.0 or later.

The software has been developed on Ubuntu 12.04 and Fedora 18. Due to compiler and library version requirements, the software may not build on earlier earlier operating system releases.

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
      libgd2-xpm-dev libboost-filesystem-dev libboost-program-options-dev

#### Mac OSX

Install [XCode](https://developer.apple.com/xcode/) and
[Homebrew](http://mxcl.github.io/homebrew/), then:

    $ brew install cmake
    $ brew install libmad
    $ brew install libsndfile
    $ brew install gd
    $ brew install boost --with-c++11

### Obtain the source code

    $ git clone https://github.com/bbcrd/audiowaveform.git
    $ cd audio_waveform

### Install googletest and googlemock test frameworks

**audiowaveform** uses [googletest](https://code.google.com/p/googletest/) and
[googlemock](https://code.google.com/p/googlemock/) for unit testing.
Following [this advice](https://code.google.com/p/googletest/wiki/V1_6_FAQ#Why_is_it_not_recommended_to_install_a_pre-compiled_copy_of_Goog)
in the googletest FAQ, download the sources and unzip:

    $ wget https://googletest.googlecode.com/files/gtest-1.6.0.zip
    $ unzip gtest-1.6.0.zip
    $ ln -s gtest-1.6.0 gtest

And similarly, for googlemock:

    $ wget https://googlemock.googlecode.com/files/gmock-1.6.0.zip
    $ unzip gmock-1.6.0.zip
    $ ln -s gmock-1.6.0 gmock

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

By default this installs the `audiowaveform` program in `/usr/local/bin`, and man pages in `/usr/local/share/man`. To change these locations, add a `-D CMAKE_INSTALL_PREFIX=...` option when invoking `cmake` above.

### Run

    $ audiowaveform --help

## Command line options

**audiowaveform** accepts the following command-line options:

| Short           | Long                           | Description                                                                                |
| --------------- | ------------------------------ | ------------------------------------------------------------------------------------------ |
|                 | `--help`                       | Show help message                                                                          |
| `-v`            | `--version`                    | Show version information                                                                   |
| `-i <filename>` | `--input-filename <filename>`  | Input audio (.wav or .mp3) or waveform data (.dat) file name                               |
| `-o <filename>` | `--output-filename <filename>` | Output waveform data (.dat or .json), audio (.wav), or PNG image (.png) filename           |
| `-s <seconds>`  | `--start <seconds>`            | Start time (seconds), default: 0                                                           |
| `-z <level>`    | `--zoom <level>`               | Zoom level (samples per pixel), default: 256                                               |
| `-b <bits>`     | `--bits <bits>`                | Number of bits resolution when creating a waveform data file (either 8 or 16), default: 16 |
| `-w <width>`    | `--width <width>`              | Width of output image (pixels), default: 800                                               |
| `-h <height>`   | `--height <height>`            | Height of output image (pixels), default: 250                                              |
| `-c <scheme>`   | `--colors <scheme>`            | Color scheme of output image (either 'audition' or 'audacity'), default: audacity          |

### Examples

Generate waveform data from an MP3 file, at 256 samples per point
with 8-bit resolution:

    $ audiowaveform -i test.mp3 -o test.dat -z 256 -b 8

Generate a 1000x200 pixel PNG image from a waveform data file,
at 512 samples per pixel, starting at 5.0 seconds from the start of the audio:

    $ audiowaveform -i test.dat -o test.png -z 512 -s 5.0 -w 1000 -h 200

Convert a waveform data file to JSON format:

    $ audiowaveform -i test.dat -o test.json

Convert MP3 to WAV format audio:

    $ audiowaveform -i test.mp3 -o test.wav

## Credits

This program contains code from the following open-source projects, used under
the terms of these projects' respective licenses:

* [Audacity](http://audacity.sourceforge.net/)
* [madlld](http://www.bsd-dk.dk/~elrond/audio/madlld/)

## License

See COPYING for details.

## Contributing

If you have a feature request or want to report a bug, we'd be happy to hear from you. Please either [raise an issue](https://github.com/bbcrd/audiowaveform/issues), or fork the project and send us a pull request.

## Authors

This software was written by [Chris Needham](https://github.com/chrisn), chris.needham at bbc.co.uk.

## Copyright

Copyright 2013 British Broadcasting Corporation
