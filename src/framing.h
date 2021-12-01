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

#ifndef SRC_FRAMING_H_
#define SRC_FRAMING_H_

#if defined(ARDUINO)
#include <Arduino.h>
#endif
#include <cstring>
#include <cstdint>
#include "checksum.h"  // NOLINT

namespace bfs {

template<std::size_t PAYLOAD_SIZE>
class FrameEncoder {
 public:
  std::size_t Write(uint8_t const * data, const std::size_t len) {
    if ((len == 0) || (!data) || (len > PAYLOAD_SIZE)) {return 0;}
    /* Reset frame position and checksum */
    frame_pos_ = 0;
    fletcher16.Reset();
    /* Frame start */
    buffer_[frame_pos_++] = FRAME_BYTE_;
    /* Payload */
    std::size_t bytes_written;
    for (bytes_written = 0; bytes_written < len; bytes_written++) {
      /* Update checksum */
      checksum_ = fletcher16.Update(&data[bytes_written], 1);
      /* Write payload */
      if ((data[bytes_written] == FRAME_BYTE_) ||
          (data[bytes_written] == ESC_BYTE_)) {
        buffer_[frame_pos_++] = ESC_BYTE_;
        buffer_[frame_pos_++] = data[bytes_written] ^ INVERT_BYTE_;
      } else {
        buffer_[frame_pos_++] = data[bytes_written];
      }
    }
    /* Checksum */
    checksum_bytes_[0] = checksum_ & 0xFF;
    checksum_bytes_[1] = checksum_ >> 8 & 0xFF;
    for (std::size_t i = 0; i < sizeof(checksum_bytes_); i++) {
      if ((checksum_bytes_[i] == FRAME_BYTE_) ||
          (checksum_bytes_[i] == ESC_BYTE_)) {
        buffer_[frame_pos_++] = ESC_BYTE_;
        buffer_[frame_pos_++] = checksum_bytes_[i] ^ INVERT_BYTE_;
      } else {
        buffer_[frame_pos_++] = checksum_bytes_[i];
      }
    }
    /* Frame end */
    buffer_[frame_pos_++] = FRAME_BYTE_;
    return bytes_written;
  }
  std::size_t size() const {
    return frame_pos_;
  }
  const uint8_t *data() const {
    return buffer_;
  }

 private:
  /* Header and footer */
  static const unsigned int HEADER_LEN_ = 1;
  static const unsigned int MAX_FOOTER_LEN_ = 5;
  /* Data buffer */
  uint8_t buffer_[2 * PAYLOAD_SIZE + HEADER_LEN_ + MAX_FOOTER_LEN_];
  /* Frame position */
  std::size_t frame_pos_ = 0;
  /* Framing */
  static const uint8_t FRAME_BYTE_ = 0x7E;
  static const uint8_t ESC_BYTE_ = 0x7D;
  static const uint8_t INVERT_BYTE_ = 0x20;
  /* Checksum */
  uint16_t checksum_;
  uint8_t checksum_bytes_[2];
  Fletcher16 fletcher16;
};

template<std::size_t PAYLOAD_SIZE>
class FrameDecoder {
 public:
  bool Found(uint8_t byte) {
    if (frame_pos_ == 0) {
      if (byte == FRAME_BYTE_) {
        buffer_[frame_pos_++] = byte;
      }
    } else {
      if (byte == FRAME_BYTE_) {
        /* frame end */
        if (frame_pos_ == 1) {
          /* mixup between start and end of packets */
        } else if (frame_pos_ >= HEADER_LEN_ + FOOTER_LEN_ - 1) {
          /* check the checksums */
          checksum_ = fletcher16.Compute(&buffer_[1], frame_pos_ - FOOTER_LEN_);
          uint16_t received_checksum =
            static_cast<uint16_t>(buffer_[frame_pos_ - 1]) << 8 |
                                  buffer_[frame_pos_ - 2];
          /* good packet */
          if (checksum_ == received_checksum) {
            /* +1 because we didn't step fpos */
            msg_len_ = frame_pos_ - HEADER_LEN_ - FOOTER_LEN_ + 1;
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
      } else if (frame_pos_ >= BUFFER_SIZE_) {
        frame_pos_ = 0;
        esc_ = false;
      } else {
        /* read into buffer */
        buffer_[frame_pos_++] = byte;
      }
    }
    return false;
  }
  std::size_t available() const {
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
    msg_len_ -= len;
    return len;
  }
  const uint8_t *data() const {return &buffer_[read_pos_];}
  std::size_t size() const {return msg_len_;}

 private:
  /* Header and footer */
  static const unsigned int HEADER_LEN_ = 1;
  static const unsigned int FOOTER_LEN_ = 3;
  static const unsigned int MAX_FOOTER_LEN_ = 5;
  /* Data buffer */
  static const unsigned int BUFFER_SIZE_ = 2 * PAYLOAD_SIZE + HEADER_LEN_
                                           + MAX_FOOTER_LEN_;
  uint8_t buffer_[BUFFER_SIZE_];
  /* Frame position */
  std::size_t frame_pos_ = 0;
  std::size_t read_pos_ = 0;
  /* Framing */
  bool esc_ = false;
  std::size_t msg_len_ = 0;
  static const uint8_t FRAME_BYTE_ = 0x7E;
  static const uint8_t ESC_BYTE_ = 0x7D;
  static const uint8_t INVERT_BYTE_ = 0x20;
  /* Checksum */
  uint16_t checksum_;
  Fletcher16 fletcher16;
};

}  // namespace bfs

#endif  // SRC_FRAMING_H_
