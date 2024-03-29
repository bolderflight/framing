/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2022 Bolder Flight Systems Inc
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

#include "framing.h"

void setup() {
  /* Create a encoder class instance with a 200 byte payload buffer */
  bfs::FrameEncoder<200> encoder;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  /* Write 10 bytes of data into the buffer */
  size_t bytes_written = encoder.Write(data, sizeof(data));
  Serial.print("Payload bytes written: ");
  Serial.println(bytes_written);
  Serial.print("Encoder buffer size: ");
  Serial.println(encoder.size());
  /* Create a decoder class instance with a 200 byte payload buffer */
  bfs::FrameDecoder<200> decoder;
  uint8_t read[200];
  /* Search for a good frame */
  for (size_t i = 0; i < 200; i++) {
    /* Data packet found */
    if (decoder.Found(*(encoder.data() + i))) {
      size_t bytes_read = decoder.Read(read, sizeof(read));
      Serial.print("Payload bytes read: ");
      Serial.println(bytes_read);
      Serial.println("Payload bytes: ");
      for (size_t i = 0; i < bytes_read; i++) {
        Serial.println(read[i]);
      }
    }
  }
}

void loop() {}
