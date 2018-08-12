/*

  An implementation of microsofts random algorithm, used for DVD padding
  sectors.
  The original algorithm to fill a random buffer works like this:

  * Finish if buffer size is zero
  * If the buffer address is uneven:
    *(uint8_t*)&p[i] = Random() >> 14; i += 1;
  * While still having to output >= 2 bytes:
    *(uint16_t*)&p[i] = Random() >> 8; i += 2;
  * If there is one byte left:
    *(uint8_t*)&p[1] = Random() >> 12; i += 1;

  Note that the last byte is written to the second byte because of a bug
  in their code.
  The value of the last random buffer byte is actually undefined.

*/

#include <cstdint>

class Random {

private:

const uint32_t b_seeds[8] = {
  0x52F690D5,
  0x534D7DDE,
  0x5B71A70F,
  0x66793320,
  0x9B7E5ED5,
  0xA465265E,
  0xA53F1D11,
  0xB154430F
};

uint32_t a;
uint32_t b;
uint32_t c;

public:

inline uint32_t Value(void) {
  uint64_t result;
  result = c;
  result += 1;
  result *= b;
  result %= 0xFFFFFFFB;
  c = result & 0xFFFFFFFF;
  return c ^ a;
}

inline size_t Fill(uint8_t* buffer, size_t length, bool odd) {
  size_t written = 0;

  // Try to reach an even address
  if (odd) {
    *buffer++ = Value() >> 14;
    length--;
    written++;
  }

  // Fill the main section
  uint16_t* word_buffer = reinterpret_cast<uint16_t*>(buffer);
  while(length >= 2) {
    *word_buffer++ = Value() >> 8;
    length -= 2;
    written += 2;
  }

  // Note that this uses the original pointer (pointing at index 1).
  // A bug in Microsofts code.
  if (length > 0) {
    *buffer = Value() >> 12;
    length--;
  }
  return written;
}

inline void Seed(uint32_t seed) {
  a = 0;
  b = b_seeds[seed & 7];
  c = seed;
  a = Value();
}

};
