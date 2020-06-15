/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2020 Bolder Flight Systems
*/

#ifndef INCLUDE_FRAMING_FRAMING_H_
#define INCLUDE_FRAMING_FRAMING_H_

#include <cstdint>
#include "checksum/checksum.h"

namespace framing {

template<std::size_t BUFFER_SIZE>
class Transmit {
 public:
  void Write(uint8_t *data, std::size_t len) {
    /* Reset frame position and checksum */
    frame_pos_ = 0;
    fletcher16.Reset();
    /* Frame start */
    buffer_[frame_pos_++] = FRAME_BYTE_;
    /* Payload */
    for (std::size_t i = 0; i < len; i++) {
      checksum_ = fletcher16.Increment(&data[i], 1);
      if ((data[i] == FRAME_BYTE_) || (data[i] == ESC_BYTE_)) {
        buffer_[frame_pos_++] = ESC_BYTE_;
        buffer_[frame_pos_++] = data[i] ^ INVERT_BYTE_;
      } else {
        buffer_[frame_pos_++] = data[i];
      }
    }
    /* Checksum */
    checksum_bytes_[0] = checksum_ & 0xFF;
    checksum_bytes_[1] = checksum_ >> 8 & 0xFF;
    for (std::size_t i = 0; i < sizeof(checksum_bytes_); i++) {
      if ((checksum_bytes_[i] == FRAME_BYTE_) || (checksum_bytes_[i] == ESC_BYTE_)) {
        buffer_[frame_pos_++] = ESC_BYTE_;
        buffer_[frame_pos_++] = checksum_bytes_[i] ^ INVERT_BYTE_;
      } else {
        buffer_[frame_pos_++] = checksum_bytes_[i];
      }
    }
    /* Frame end */
    buffer_[frame_pos_++] = FRAME_BYTE_;
  }
  std::size_t Size() {
    return frame_pos_;
  }
  uint8_t *Data() {
    return buffer_;
  }

 private:
  /* Data buffer */ 
  uint8_t buffer_[BUFFER_SIZE];
  /* Frame position */
  std::size_t frame_pos_ = 0;
  /* Framing */
  static const uint8_t FRAME_BYTE_ = 0x7E;
  static const uint8_t ESC_BYTE_ = 0x7D;
  static const uint8_t INVERT_BYTE_ = 0x20;
  /* Checksum */
  uint16_t checksum_;
  uint8_t checksum_bytes_[2];
  checksum::Fletcher16 fletcher16;
};

template<std::size_t BUFFER_SIZE>
class Receive {
 public:
  bool Found(uint8_t byte) {
    // std::cout << frame_pos_ << std::endl;
    if (frame_pos_ == 0) {
      if (byte == FRAME_BYTE_) {
        buffer_[frame_pos_++] = byte;
      }
    } else {
      if (byte == FRAME_BYTE_) {
        /* frame end */
        if (frame_pos_ == 1) {
          // Do nothing
        } else if (frame_pos_ >= HEADER_LEN_ + FOOTER_LEN_ - 1) {
          /* passed crc check, good packet */
          checksum_ = fletcher16.Compute(&buffer_[1], frame_pos_ - FOOTER_LEN_);
          uint16_t received_checksum = static_cast<uint16_t>(buffer_[frame_pos_ - 1]) << 8 | buffer_[frame_pos_ - 2];
          if (checksum_ == received_checksum) {
            msg_len_ = frame_pos_ - HEADER_LEN_ - FOOTER_LEN_ + 1; // +1 because we didn't step fpos
            read_pos_ = HEADER_LEN_;
            frame_pos_ = 0;
            esc_ = false;
            return true;
          /* did not pass checksum, bad packet */
          } else {
            frame_pos_ = 0;
            esc_ = false;
            return false;
          }
        /* bad frame */
        } else {
          frame_pos_ = 0;
          esc_ = false;
          return false;
        }
      } else if (byte == ESC_BYTE_) {
        esc_ = true;
      } else if (esc_) {
        byte = byte ^ INVERT_BYTE_;
        esc_ = false;
        /* read into buffer */
        buffer_[frame_pos_++] = byte;
      /* prevent buffer overflow */
      } else if (frame_pos_ >= BUFFER_SIZE) {
        frame_pos_ = 0;
        esc_ = false;
      } else {
        /* read into buffer */
        buffer_[frame_pos_++] = byte;
      }
    }
    return false;
  }
  uint8_t Available() {
    return msg_len_;
  }
  uint8_t Read() {
    if (msg_len_ > 0) {
      msg_len_--;
      return buffer_[read_pos_++];
    } else {
      return 0;
    }
  }
  std::size_t Read(uint8_t *data, std::size_t len) {
    if ((len == 0) || (!data)) {
      return 0;
    }
    if (len > msg_len_) {
      len = msg_len_;
    }
    memcpy(data, &buffer_[read_pos_], len);
    read_pos_ += len;
    msg_len_ -=len;
    return len;
  }

 private:
  /* Data buffer */ 
  uint8_t buffer_[BUFFER_SIZE];
  /* Frame position */
  std::size_t frame_pos_ = 0;
  std::size_t read_pos_ = 0;
  /* Framing */
  bool esc_ = false;
  std::size_t msg_len_ = 0;
  static const uint8_t FRAME_BYTE_ = 0x7E;
  static const uint8_t ESC_BYTE_ = 0x7D;
  static const uint8_t INVERT_BYTE_ = 0x20;
  /* Header and footer */
  static const unsigned int HEADER_LEN_ = 1;
  static const unsigned int FOOTER_LEN_ = 3;
  /* Checksum */
  uint16_t checksum_;
  checksum::Fletcher16 fletcher16;
};

}  // namespace framing

#endif  // INCLUDE_FRAMING_FRAMING_H_
