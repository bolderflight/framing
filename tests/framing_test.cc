/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2020 Bolder Flight Systems
*/

#include "framing/framing.h"
#include "gtest/gtest.h"
#include "framing/framing.h"

/* Test encoder nullptr */
TEST(Encoder, NullPtr) {
  framing::Encoder<10> encoder;
  EXPECT_EQ(0, encoder.Write(nullptr, 10));
}
/* Test Encoder 0 len */
TEST(Encoder, Len0) {
  framing::Encoder<10> encoder;
  uint8_t data[0];
  EXPECT_EQ(0, encoder.Write(data, sizeof(data)));
}
/* Test Encoder, buffer too small */
TEST(Encoder, SmallBuff) {
  framing::Encoder<5> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  EXPECT_EQ(0, encoder.Write(data, sizeof(data)));
}
/* Test Encoder, good inputs */
TEST(Encoder, Good) {
  framing::Encoder<100> encoder;
  checksum::Fletcher16 chk;
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
  framing::Decoder<100> decoder;
  EXPECT_EQ(0, decoder.Read(nullptr, 10));
}
/* Test Decoder 0 len */
TEST(Decoder, Len0) {
  framing::Decoder<100> decoder;
  uint8_t data[0];
  EXPECT_EQ(0, decoder.Read(data, sizeof(data)));
}
/* Test good decoder */
TEST(Decoder, Good) {
  framing::Encoder<100> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[20];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  encoder.Write(data, sizeof(data));
  framing::Decoder<100> decoder;
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
  framing::Encoder<100> encoder;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[5];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  encoder.Write(data, sizeof(data));
  framing::Decoder<100> decoder;
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
  framing::Decoder<100> decoder;
  uint8_t data[10];
  EXPECT_EQ(0, decoder.Available());
  EXPECT_EQ(0, decoder.Read());
  EXPECT_EQ(0, decoder.Read(data, sizeof(data)));
}
