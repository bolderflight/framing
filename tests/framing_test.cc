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

#include "framing/framing.h"
#include "gtest/gtest.h"
#include "framing/framing.h"

/* Test encoder nullptr */
TEST(Encoder, NullPtr) {
  bfs::Encoder<10> encoder;
  EXPECT_EQ(0, encoder.Write(nullptr, 10));
}
/* Test Encoder 0 len */
TEST(Encoder, Len0) {
  bfs::Encoder<10> encoder;
  uint8_t data[0];
  EXPECT_EQ(0, encoder.Write(data, sizeof(data)));
}
/* Test Encoder, buffer too small */
TEST(Encoder, SmallBuff) {
  bfs::Encoder<5> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  EXPECT_EQ(0, encoder.Write(data, sizeof(data)));
}
/* Test Encoder, good inputs */
TEST(Encoder, Good) {
  bfs::Encoder<100> encoder;
  bfs::Fletcher16 chk;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint16_t checksum = chk.Compute(data, sizeof(data));
  EXPECT_EQ(10, encoder.Write(data, sizeof(data)));
  EXPECT_EQ(14, encoder.Size());
  EXPECT_EQ(126, *(encoder.Data() + 0));
  EXPECT_EQ(1, *(encoder.Data() + 1));
  EXPECT_EQ(2, *(encoder.Data() + 2));
  EXPECT_EQ(3, *(encoder.Data() + 3));
  EXPECT_EQ(4, *(encoder.Data() + 4));
  EXPECT_EQ(5, *(encoder.Data() + 5));
  EXPECT_EQ(6, *(encoder.Data() + 6));
  EXPECT_EQ(7, *(encoder.Data() + 7));
  EXPECT_EQ(8, *(encoder.Data() + 8));
  EXPECT_EQ(9, *(encoder.Data() + 9));
  EXPECT_EQ(10, *(encoder.Data() + 10));
  EXPECT_EQ(checksum & 0xFF, *(encoder.Data() + 11));
  EXPECT_EQ(checksum >> 8 & 0xFF, *(encoder.Data() + 12));
  EXPECT_EQ(126, *(encoder.Data() + 13));
}
/* Test Decoder, null ptr */
TEST(Decoder, NullPtr) {
  bfs::Decoder<100> decoder;
  EXPECT_EQ(0, decoder.Read(nullptr, 10));
}
/* Test Decoder 0 len */
TEST(Decoder, Len0) {
  bfs::Decoder<100> decoder;
  uint8_t data[0];
  EXPECT_EQ(0, decoder.Read(data, sizeof(data)));
}
/* Test good decoder */
TEST(Decoder, Good) {
  bfs::Encoder<100> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[20];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  encoder.Write(data, sizeof(data));
  bfs::Decoder<100> decoder;
  for (std::size_t i = 0; i < encoder.Size(); i++) {
    if (decoder.Found(*(encoder.Data() + i))) {
      bytes_avail = decoder.Available();
      bytes_read = decoder.Read(read_data, sizeof(read_data));
    }
  }
  EXPECT_EQ(sizeof(data), bytes_avail);
  EXPECT_EQ(sizeof(data), bytes_read);
  for (std::size_t i = 0; i < sizeof(data); i++) {
    EXPECT_EQ(data[i], read_data[i]);
  }
}
/* Test Decoder smaller buffer */
TEST(Decoder, SmallBuff) {
  bfs::Encoder<100> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[5];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  encoder.Write(data, sizeof(data));
  bfs::Decoder<100> decoder;
  for (std::size_t i = 0; i < encoder.Size(); i++) {
    if (decoder.Found(*(encoder.Data() + i))) {
      bytes_avail = decoder.Available();
      bytes_read = decoder.Read(read_data, sizeof(read_data));
    }
  }
  EXPECT_EQ(sizeof(data), bytes_avail);
  EXPECT_EQ(5, bytes_read);
  for (std::size_t i = 0; i < 5; i++) {
    EXPECT_EQ(data[i], read_data[i]);
  }
  for (std::size_t i = 5; i < 10; i++) {
    EXPECT_EQ(data[i], decoder.Read());
  }
}
/* Test Decoder empty */
TEST(Decoder, Empty) {
  bfs::Decoder<100> decoder;
  uint8_t data[10];
  EXPECT_EQ(0, decoder.Available());
  EXPECT_EQ(0, decoder.Read());
  EXPECT_EQ(0, decoder.Read(data, sizeof(data)));
}
