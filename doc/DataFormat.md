# Waveform data file format

This page describes the binary and JSON data formats produced by the **audiowaveform (1)**
program.

## Binary data format (.dat)

**audiowaveform** expects binary waveform data files to use the `.dat`
file extension. This format consists of a header block followed by the
actual waveform data. All values are little-endian.

The header block starts with a version field that identifies how
the rest of the data in the file is structured.

| Byte offset | Type     | Field             |
| ----------- | -------- | ----------------- |
| 0-3         | int32_t  | Version           |

The version 1 header is structured as follows:

| Byte offset | Type     | Field             |
| ----------- | -------- | ----------------- |
| 4-7         | uint32_t | Flags             |
| 8-11        | int32_t  | Sample rate       |
| 12-15       | int32_t  | Samples per pixel |
| 16-19       | uint32_t | Length            |

The version 2 header is structured as follows:

| Byte offset | Type     | Field             |
| ----------- | -------- | ----------------- |
| 4-7         | uint32_t | Flags             |
| 8-11        | int32_t  | Sample rate       |
| 12-15       | int32_t  | Samples per pixel |
| 16-19       | uint32_t | Length            |
| 20-23       | int32_t  | Channels          |

Each of these fields is described in detail below.

### Version

This field indicates the version number of the waveform data format. The version
1 and 2 data formats are described here. If the format changes in future, the
Version field will be incremented.

### Flags

The Flags field describes the format of the waveform data that follows the
header.

| Bit     | Description                               |
| ------- | ----------------------------------------- |
| 0 (lsb) | 0: 16-bit resolution, 1: 8-bit resolution |
| 1-31    | Unused                                    |

### Sample rate

Sample rate of the original audio file (Hz).

### Samples per pixel

Number of audio samples per waveform minimum/maximum pair.

### Length

Length of waveform data (number of minimum and maximum value pairs per channel).

### Channels

The number of waveform channels present (version 2 only).

### Waveform data

Waveform data follows the header block and consists of pairs of minimum and
maximum values that each represent a range of samples of the original audio (the
"samples per pixel" header field).

The version 1 data format supports only a single audio channel; the
**audiowaveform** program converts stereo audio to mono when generating
waveform data. The version 2 data format supports multiple channels, where the
data from each channel is interleaved.

For 8-bit data, the waveform data is represented as follows. Each value lies in
the range -128 to +127. The example shows a two channel waveform data file.

| Byte offset | Type   | Value                                    |
| ----------- | ------ | ---------------------------------------- |
| 20          | int8_t | Minimum sample value, index 0, channel 0 |
| 21          | int8_t | Maximum sample value, index 0, channel 0 |
| 22          | int8_t | Minimum sample value, index 0, channel 1 |
| 23          | int8_t | Maximum sample value, index 0, channel 1 |
| 24          | int8_t | Minimum sample value, index 1, channel 0 |
| 25          | int8_t | Maximum sample value, index 1, channel 0 |
| 26          | int8_t | Minimum sample value, index 1, channel 1 |
| 27          | int8_t | Maximum sample value, index 1, channel 1 |
| etc         | ...    | ...                                      |

Pairs of minimum and maximum values repeat to the end of the file.

For 16-bit data, the waveform data is represented as follows. Each value lies in
the range -32768 to +32767. The example shows a two channel waveform data file.

| Byte offset | Type    | Value                                    |
| ----------- | ------- | ---------------------------------------- |
| 20-21       | int16_t | Minimum sample value, index 0, channel 0 |
| 22-23       | int16_t | Maximum sample value, index 0, channel 0 |
| 24-25       | int16_t | Minimum sample value, index 0, channel 1 |
| 25-26       | int16_t | Maximum sample value, index 0, channel 1 |
| 27-28       | int16_t | Minimum sample value, index 1, channel 0 |
| 29-30       | int16_t | Maximum sample value, index 1, channel 0 |
| 31-32       | int16_t | Minimum sample value, index 1, channel 1 |
| 33-34       | int16_t | Maximum sample value, index 1, channel 1 |
| etc         | ...     | ...                                      |

Pairs of minimum and maximum values repeat to the end of the file.

## JSON data format (.json)

The JSON data format contains the same information as the binary format.
**audiowaveform** expects JSON data files to use the `.json` file extension.
The format consists of a single JSON object containing fields as described
below.

### version

The version number of the waveform data format. The version 1 and 2 data formats
are described here. If the format changes in future, the version field will be
incremented.

### channels

The number of waveform channels present (version 2 only).

### sample_rate

Sample rate of the original audio file (Hz).

### samples_per_pixel

Number of audio samples per waveform minimum/maximum pair.

### bits

Resolution of waveform data. May be either 8 or 16.

### length

Length of waveform data (number of minimum and maximum value pairs per channel).

### data

Array of minimum and maximum waveform data points, interleaved.
The example shows a two channel waveform data file.
Depending on `bits`, each value may be in the range -128 to +127
or -32768 to +32767.

| Array offset | Value                                    |
| ------------ | ---------------------------------------- |
| 0            | Minimum sample value, index 0, channel 0 |
| 1            | Maximum sample value, index 0, channel 0 |
| 2            | Minimum sample value, index 0, channel 1 |
| 3            | Maximum sample value, index 0, channel 1 |
| 4            | Minimum sample value, index 1, channel 0 |
| 5            | Maximum sample value, index 1, channel 0 |
| 6            | Minimum sample value, index 1, channel 1 |
| 7            | Maximum sample value, index 1, channel 1 |
| etc          | ...                                      |

### Example

The following is an example of a (very short) waveform data file in JSON format.

    {
      "version": 2,
      "channels": 2,
      "sample_rate": 48000,
      "samples_per_pixel": 512,
      "bits": 8,
      "length": 3,
      "data": [-65,63,-66,64,-40,41,-39,45,-55,43,-55,44]
    }
