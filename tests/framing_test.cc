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

#include "framing.h"
#include "gtest/gtest.h"

/* Test encoder nullptr */
TEST(Encoder, NullPtr) {
  bfs::FrameEncoder<10> encoder;
  EXPECT_EQ(0, encoder.Write(nullptr, 10));
}
/* Test Encoder 0 len */
TEST(Encoder, Len0) {
  bfs::FrameEncoder<10> encoder;
  uint8_t data[0];
  EXPECT_EQ(0, encoder.Write(data, sizeof(data)));
}
/* Test Encoder, buffer too small */
TEST(Encoder, SmallBuff) {
  bfs::FrameEncoder<5> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  EXPECT_EQ(0, encoder.Write(data, sizeof(data)));
}
/* Test Encoder, good inputs */
TEST(Encoder, Good) {
  bfs::FrameEncoder<100> encoder;
  bfs::Fletcher16 chk;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint16_t checksum = chk.Compute(data, sizeof(data));
  EXPECT_EQ(10, encoder.Write(data, sizeof(data)));
  EXPECT_EQ(14, encoder.size());
  EXPECT_EQ(126, *(encoder.data() + 0));
  EXPECT_EQ(1, *(encoder.data() + 1));
  EXPECT_EQ(2, *(encoder.data() + 2));
  EXPECT_EQ(3, *(encoder.data() + 3));
  EXPECT_EQ(4, *(encoder.data() + 4));
  EXPECT_EQ(5, *(encoder.data() + 5));
  EXPECT_EQ(6, *(encoder.data() + 6));
  EXPECT_EQ(7, *(encoder.data() + 7));
  EXPECT_EQ(8, *(encoder.data() + 8));
  EXPECT_EQ(9, *(encoder.data() + 9));
  EXPECT_EQ(10, *(encoder.data() + 10));
  EXPECT_EQ(checksum & 0xFF, *(encoder.data() + 11));
  EXPECT_EQ(checksum >> 8 & 0xFF, *(encoder.data() + 12));
  EXPECT_EQ(126, *(encoder.data() + 13));
}
/* Test Decoder, null ptr */
TEST(Decoder, NullPtr) {
  bfs::FrameDecoder<100> decoder;
  EXPECT_EQ(0, decoder.Read(nullptr, 10));
}
/* Test Decoder 0 len */
TEST(Decoder, Len0) {
  bfs::FrameDecoder<100> decoder;
  uint8_t data[0];
  EXPECT_EQ(0, decoder.Read(data, sizeof(data)));
}
/* Test good decoder */
TEST(Decoder, Good) {
  bfs::FrameEncoder<100> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[20];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  encoder.Write(data, sizeof(data));
  bfs::FrameDecoder<100> decoder;
  for (std::size_t i = 0; i < encoder.size(); i++) {
    if (decoder.Found(*(encoder.data() + i))) {
      bytes_avail = decoder.available();
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
  bfs::FrameEncoder<100> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[5];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  encoder.Write(data, sizeof(data));
  bfs::FrameDecoder<100> decoder;
  for (std::size_t i = 0; i < encoder.size(); i++) {
    if (decoder.Found(*(encoder.data() + i))) {
      bytes_avail = decoder.available();
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
  bfs::FrameDecoder<100> decoder;
  uint8_t data[10];
  EXPECT_EQ(0, decoder.available());
  EXPECT_EQ(0, decoder.Read());
  EXPECT_EQ(0, decoder.Read(data, sizeof(data)));
}
