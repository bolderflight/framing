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
  framing::Transmit<200> transmit;
  uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  transmit.Write(data, sizeof(data));
  std::cout << transmit.Size() << std::endl << std::endl;
  for (unsigned int i = 0; i < transmit.Size(); i++) {
    std::cout << std::to_string(*(transmit.Data() + i)) << std::endl;
  }

  framing::Receive<200> receive;
  uint8_t recv[200];
  uint8_t read[200];
  memcpy(recv, transmit.Data(), transmit.Size());
  unsigned int ret;
  for (unsigned int i = 0; i < sizeof(recv); i++) {
    if (receive.Found(recv[i])) {
      ret = receive.Read(read, sizeof(read));
      std::cout << ret << std::endl << std::endl;
    }
  }
  for (unsigned int i = 0; i < ret; i++) {
    std::cout << std::to_string(read[i]) << std::endl;
  }
}
