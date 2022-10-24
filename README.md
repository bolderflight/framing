[![Pipeline](https://gitlab.com/bolderflight/software/framing/badges/main/pipeline.svg)](https://gitlab.com/bolderflight/software/framing/) [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

![Bolder Flight Systems Logo](img/logo-words_75.png) &nbsp; &nbsp; ![Arduino Logo](img/arduino_logo_75.png)

# Framing
This library frames data payloads with a start byte, end byte, and checksum; making data suitable for serial transmission or storage and providing a framework for quickly developing messages. Classes and methods are provided for encoding and decoding data payloads. This library is compatible with Arduino and with CMake build systems. It would also be easy to include with other projects, since it is a header only library consisting of a single file.
   * [License](LICENSE.md)
   * [Changelog](CHANGELOG.md)
   * [Contributing guide](CONTRIBUTING.md)

# Installation

## Arduino
Use the Arduino Library Manager to install this library or clone to your Arduino/libraries folder. Additionally, the [Bolder Flight Systems Checksum library](https://github.com/bolderflight/checksum) must be installed. This library is added as:

```C++
#include "framing.h"
```

An example Arduino executable is located at *examples/arduino/framing_example/framing_example.ino*. Teensy 3.x, 4.x, and LC devices are used for testing under Arduino and this library should be compatible with other Arduino devices.

## CMake
CMake is used to build this library, which is exported as a library target called *framing*. The header is added as:

```C++
#include "framing.h"
```

The library can be also be compiled stand-alone using the CMake idiom of creating a *build* directory and then, from within that directory issuing:

```
cmake ..
make
```

This will build the library, an example executable called *framing_example*, and an executable for testing using the Google Test framework, called *framing_test*. The example executable source file is located at *examples/cmake/framing_example.cc*.

# Namespace
This library is within the namespace *bfs*.

# FrameEncoder
This class is used for encoding data payloads. Data payloads are framed by:
   * Header byte of 0x7E
   * Payload
   * Fletcher16 checksum
   * Footer byte of 0x7E

An escape character of 0x7D is used to escape any occurances of 0x7E or 0x7D in the payload data or checksum bytes and 0x20 is used as an inversion byte to transform the data.

## Methods

**FrameEncoder<PAYLOAD_SIZE>** Creates an encoder object, PAYLOAD_SIZE is a template parameter used to set the number of bytes in the largest payload that will be encoded. This class will reserve a buffer that is 2 * PAYLOAD_SIZE + 5 bytes long, which guarantees that all data payloads up to PAYLOAD_SIZE long can be encoded, leaving enough buffer for escape characters, checksum, header and footer bytes.

```C++
bfs::FrameEncoder<200> encoder;
```

**size_t Write(uint8_t const &ast; const data, const size_t len)** Encodes the data passed by a pointer to the payload data and length. Returns the number of payload bytes encoded.

```C++
uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
size_t bytes_written = encoder.Write(data, sizeof(data));
```

**size_t size()** Returns the size of the encoded buffer, including the header, footer, checksum, and escape bytes. This is the total size of the buffer to be transmitted or stored.

**uint8_t const &ast; const data()** Returns a pointer to the encoded buffer. Use along with the *size* method to transmit or store the encoded data.

```C++
fwrite(encoder.data(), encoder.size(), 1, fd);
```

# FrameDecoder
This class is used for decoding data payloads from encoded packets. 

## Methods

**FrameDecoder<PAYLOAD_SIZE>** Creates a decoder object, PAYLOAD_SIZE is a template parameter used to set the number of bytes in the largest payload that will be decoded. This class will reserve a buffer that is 2 * PAYLOAD_SIZE + 5 bytes long, which guarantees that all data payloads up to PAYLOAD_SIZE long can be decoded, leaving enough buffer for escape characters, checksum, header and footer bytes.

```C++
bfs::FrameDecoder<200> decoder;
```

**bool Found(uint8_t byte)** Searches for valid data payloads, given a stream of encoded bytes, returning true when one is found.

**size_t available()** Returns the size of the decoded data payload, in bytes. If some of the data payload has been read, using the *Read* methods described below, returns the number of bytes remaining in the data payload.

**uint8_t Read()** Returns a single byte from the decoded data payload.

**size_t Read(uint8_t &ast; const data, size_t len)** Copies the decoded data payload into the buffer pointed to by *data* up to *len* bytes. Returns the number of bytes read.

**uint8_t const &ast; const data()** Returns a pointer to the start of the decoded data payload.

**size_t size()**  Returns the length of the decoded data payload. Equivalent to the *available* method.

```C++
/* Create a decoder class instance with a 200 byte payload buffer */
uint8_t encoded_data[1000];
bfs::FrameDecoder<200> decoder;
uint8_t read[200];
/* Search for a good frame */
for (size_t i = 0; i < sizeof(encoded_data); i++) {
  /* Data packet found */
  if (decoder.Found(encoded_data[i])) {
    size_t bytes_read = decoder.Read(read, sizeof(read));
    std::cout << "Payload bytes read: " << bytes_read << std::endl;
    std::cout << "Payload bytes: " << std::endl;
    for (size_t i = 0; i < bytes_read; i++) {
      std::cout << std::to_string(read[i]) << std::endl;
    }
  }
}
```
