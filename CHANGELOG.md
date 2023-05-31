# Audiowaveform Version History

## v1.8.0 (2023-05-31)

 * Added support for rendering waveforms as vertical bars. Use the
   `--waveform-style bars` option, and the `--bar-style`,
   `--bar-width`, and `--bar-gap` options to customize the image

## v1.7.1 (2023-03-19)

 * Fixed waveform image generation when audio is read from a socket
   and the `--zoom auto` option is used to automatically fit the
   waveform to a given image width

## v1.7.0 (2022-12-10)

 * Fixed waveform image generation when audio is piped to stdin
   and the `--zoom auto` option is used to automatically fit the
   waveform to a given image width

## v1.6.0 (2022-02-18)

 * Added support for Opus audio
 * Fixed crash when reading malformed .dat files

## v1.5.1 (2021-07-31)

 * Fixed buffer overflow error in MP3 decoder

## v1.5.0 (2021-07-07)

 * Added `--quiet` option, to disable progress and information messages
 * Increased channel limit to 24
 * Removed sample rate, zoom, and start time limits when generating
   waveform images

## v1.4.2 (2020-05-03)

 * Enable `--amplitude-scale` option when generating waveform data

## v1.4.1 (2020-01-28)

 * Enable conversion from FLAC or Ogg Vorbis audio to WAV format

## v1.4.0 (2019-11-03)

 * Added `--input-format` and `--output-format` options, to enable
   reading input from stdin and writing output to stdout

## v1.3.3 (2018-12-11)

 * Increase channel limit to eight

## v1.3.2 (2018-11-23)

 * Fixed version 2 binary data format

## v1.3.1 (2018-11-21)

 * Added `--split-channels` option to produce multi-channel output
   files
 * Enabled static linking

## v1.2.2 (2018-06-23)

 * Fixed MinGW build
 * Updated Ubuntu package details

## v1.2.1 (2018-05-25)

 * Added support for Ogg Vorbis audio

## v1.2.0 (2018-04-16)

 * Added `--zoom` `auto` option to automatically fit the waveform to a
   given image width when generating PNG images

## v1.1.0 (2017-02-02)

 * Skip information frames in MP3 files, to correct for initial
   offset delay

## v1.0.12 (2016-12-20)

 * Added `--amplitude-scale` option to control ampltiude scaling
   when generating PNG images
 * Skip ID3 tags in MP3 files

## v1.0.11 (2016-04-22)

 * Added `--compression` command line option to set PNG compression
   level
 * Removed examples to shorten `--help` output

## v1.0.10 (2015-03-27)

 * Corrected handling of floating point format WAV files

## v1.0.9 (2014-11-12)

 * Added `--pixels-per-second` option as alternative way of setting the
   zoom level

## v1.0.8 (2014-10-22)

 * Corrected use of `--bits` option when converting waveform data files
 * Allow image colours to include transparency

## v1.0.7 (2014-09-22)

 * Added support for FLAC audio

## v1.0.6 (2014-04-01)

 * Added command-line options to set image colours

## v1.0.5 (2014-03-21)

 * Allow creation of JSON waveform data files from input audio files

## v1.0.4 (2014-03-10)

 * Added `--end` option to set time range when rendering images and
   `--no-axis-labels` option to control axis label rendering

## v1.0.3 (2013-12-11)

 * Allow creation of PNG images directly from input audio files

## v1.0.2 (2013-12-11)

 * Added support for mono input audio files

## v1.0.1 (2013-10-14)

 * Initial public release
