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
  /* Create a transmit class instance with a 200 byte buffer */
  framing::Transmit<200> transmit;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  /* Write 10 bytes of data into the buffer */
  std::size_t bytes_written = transmit.Write(data, sizeof(data));
  std::cout << "Payload bytes written: " << bytes_written << std::endl;
  std::cout << "Transmit buffer size: " << transmit.Size() << std::endl;
  /* Create a receive class instance witha  200 byte buffer */
  framing::Receive<200> receive;
  uint8_t read[200];
  /* Search for a good frame */
  for (std::size_t i = 0; i < 200; i++) {
    /* Data packet found */
    if (receive.Found(*(transmit.Data() + i))) {
      std::size_t bytes_read = receive.Read(read, sizeof(read));
      std::cout << "Payload bytes read: " << bytes_read << std::endl;
      std::cout << "Payload bytes: " << std::endl;
      for (std::size_t i = 0; i < bytes_read; i++) {
        std::cout << std::to_string(read[i]) << std::endl;
      }
    }
  }
}
