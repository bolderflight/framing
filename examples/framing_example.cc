/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2020 Bolder Flight Systems
*/

#include <string.h>
#include <iostream>
#include "framing/framing.h"

int main() {
  /* Create a encoder class instance with a 200 byte payload buffer */
  framing::Encoder<200> encoder;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  /* Write 10 bytes of data into the buffer */
  std::size_t bytes_written = encoder.Write(data, sizeof(data));
  std::cout << "Payload bytes written: " << bytes_written << std::endl;
  std::cout << "Encoder buffer size: " << encoder.Size() << std::endl;
  /* Create a decoder class instance with a 200 byte payload buffer */
  framing::Decoder<200> decoder;
  uint8_t read[200];
  /* Search for a good frame */
  for (std::size_t i = 0; i < 200; i++) {
    /* Data packet found */
    if (decoder.Found(*(encoder.Data() + i))) {
      std::size_t bytes_read = decoder.Read(read, sizeof(read));
      std::cout << "Payload bytes read: " << bytes_read << std::endl;
      std::cout << "Payload bytes: " << std::endl;
      for (std::size_t i = 0; i < bytes_read; i++) {
        std::cout << std::to_string(read[i]) << std::endl;
      }
    }
  }
}
