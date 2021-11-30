/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2021 Bolder Flight Systems Inc
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#include <string.h>
#include <iostream>
#include "framing.h"

int main() {
  /* Create a encoder class instance with a 200 byte payload buffer */
  bfs::FrameEncoder<200> encoder;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  /* Write 10 bytes of data into the buffer */
  std::size_t bytes_written = encoder.Write(data, sizeof(data));
  std::cout << "Payload bytes written: " << bytes_written << std::endl;
  std::cout << "Encoder buffer size: " << encoder.size() << std::endl;
  /* Create a decoder class instance with a 200 byte payload buffer */
  bfs::FrameDecoder<200> decoder;
  uint8_t read[200];
  /* Search for a good frame */
  for (std::size_t i = 0; i < 200; i++) {
    /* Data packet found */
    if (decoder.Found(*(encoder.data() + i))) {
      std::size_t bytes_read = decoder.Read(read, sizeof(read));
      std::cout << "Payload bytes read: " << bytes_read << std::endl;
      std::cout << "Payload bytes: " << std::endl;
      for (std::size_t i = 0; i < bytes_read; i++) {
        std::cout << std::to_string(read[i]) << std::endl;
      }
    }
  }
}
