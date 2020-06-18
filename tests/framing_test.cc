/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2020 Bolder Flight Systems
*/

#include "framing/framing.h"
#include "gtest/gtest.h"
#include "framing/framing.h"

/* Test transmit nullptr */
TEST(Transmit, NullPtr) {
  framing::Transmit<10> writer;
  EXPECT_EQ(0, writer.Write(nullptr, 10));
}
/* Test transmit 0 len */
TEST(Transmit, Len0) {
  framing::Transmit<10> writer;
  uint8_t data[0];
  EXPECT_EQ(0, writer.Write(data, sizeof(data)));
}
/* Test transmit, buffer too small */
TEST(Transmit, SmallBuff) {
  framing::Transmit<10> writer;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  EXPECT_EQ(4, writer.Write(data, sizeof(data)));
}
/* Test transmit, good inputs */
TEST(Transmit, Good) {
  framing::Transmit<100> writer;
  checksum::Fletcher16 chk;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint16_t checksum = chk.Compute(data, sizeof(data));
  EXPECT_EQ(10, writer.Write(data, sizeof(data)));
  EXPECT_EQ(14, writer.Size());
  EXPECT_EQ(126, *(writer.Data() + 0));
  EXPECT_EQ(1, *(writer.Data() + 1));
  EXPECT_EQ(2, *(writer.Data() + 2));
  EXPECT_EQ(3, *(writer.Data() + 3));
  EXPECT_EQ(4, *(writer.Data() + 4));
  EXPECT_EQ(5, *(writer.Data() + 5));
  EXPECT_EQ(6, *(writer.Data() + 6));
  EXPECT_EQ(7, *(writer.Data() + 7));
  EXPECT_EQ(8, *(writer.Data() + 8));
  EXPECT_EQ(9, *(writer.Data() + 9));
  EXPECT_EQ(10, *(writer.Data() + 10));
  EXPECT_EQ(checksum & 0xFF, *(writer.Data() + 11));
  EXPECT_EQ(checksum >> 8 & 0xFF, *(writer.Data() + 12));
  EXPECT_EQ(126, *(writer.Data() + 13));
}
/* Test receive, null ptr */
TEST(Receive, NullPtr) {
  framing::Receive<100> reader;
  EXPECT_EQ(0, reader.Read(nullptr, 10));
}
/* Test receive 0 len */
TEST(Receive, Len0) {
  framing::Receive<100> reader;
  uint8_t data[0];
  EXPECT_EQ(0, reader.Read(data, sizeof(data)));
}
/* Test good receive */
TEST(Receive, Good) {
  framing::Transmit<100> writer;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[20];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  writer.Write(data, sizeof(data));
  framing::Receive<100> reader;
  for (std::size_t i = 0; i < writer.Size(); i++) {
    if (reader.Found(*(writer.Data() + i))) {
      bytes_avail = reader.Available();
      bytes_read = reader.Read(read_data, sizeof(read_data));
    }
  }
  EXPECT_EQ(sizeof(data), bytes_avail);
  EXPECT_EQ(sizeof(data), bytes_read);
  for (std::size_t i = 0; i < sizeof(data); i++) {
    EXPECT_EQ(data[i], read_data[i]);
  }
}
/* Test receive smaller buffer */
TEST(Receive, SmallBuff) {
  framing::Transmit<100> writer;
  uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
  uint8_t read_data[5];
  std::size_t bytes_avail = 0;
  std::size_t bytes_read = 0;
  writer.Write(data, sizeof(data));
  framing::Receive<100> reader;
  for (std::size_t i = 0; i < writer.Size(); i++) {
    if (reader.Found(*(writer.Data() + i))) {
      bytes_avail = reader.Available();
      bytes_read = reader.Read(read_data, sizeof(read_data));
    }
  }
  EXPECT_EQ(sizeof(data), bytes_avail);
  EXPECT_EQ(5, bytes_read);
  for (std::size_t i = 0; i < 5; i++) {
    EXPECT_EQ(data[i], read_data[i]);
  }
  for (std::size_t i = 5; i < 10; i++) {
    EXPECT_EQ(data[i], reader.Read());
  }
}
