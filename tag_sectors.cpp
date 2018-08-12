#include <cstdint>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>

#include "random.h"

const uint64_t data_area_offset = 0x18300000LLU;

bool BruteforceSeed(const uint8_t* buffer, uint64_t size, uint32_t* seed) {
  assert(size % 2 == 0);
  volatile bool complete = false;
  std::printf("Starting bruteforce\n");
  #pragma omp parallel
  {
    Random rnd;
    #pragma omp for
    for(uint64_t i = 0; i <= 0xFFFFFFFF; i++) {
      if (complete) {
        continue;
      }
      rnd.Seed(i);
      const uint16_t* a = reinterpret_cast<const uint16_t*>(buffer);
      uint64_t j;
      for(j = 0; j < size; j += 2) {
        uint16_t b = rnd.Value() >> 8;
        if (*a++ != b) {
          break;
        }
      }
      if (j == size) {
        #pragma omp critical
        {
          std::printf("Found seed 0x%08" PRIX32 "\n", i);
          if (seed) {
            *seed = i;
          }
          complete = true;
        }
      }
    }
  }
  std::printf("Completed bruteforce (%s)\n", complete ? "Success" : "Failure");
  return complete;
}

int main(int argc, char* argv[]) {
  uint32_t seed;
  uint8_t random_buffer[2048];
  uint8_t sector_buffer[2048];

  std::FILE* f = std::fopen(argv[1], "rb"); 

  // Skip to data partition
  //FIXME: Get offset from SS

  // Measure file length
  std::fseek(f, 0, SEEK_END);
  uint64_t file_length = std::ftell(f);

  // Bruteforce the seed by matching the first sector
  std::fseek(f, data_area_offset, SEEK_SET);
  fread(random_buffer, 1, 2048, f);
  if (!BruteforceSeed(random_buffer, 2048, &seed)) {
    std::fprintf(stderr, "Unable to find a seed\n");
    return 1;
  }

  // Now initialize the actual random generator and get first sector
  Random rnd;
  rnd.Seed(seed);
  size_t written = rnd.Fill(random_buffer, 2048, false);
  assert(written == 2048);

  // Seek back to the first data sector
  std::fseek(f, data_area_offset, SEEK_SET);

  uint64_t random_bytes = 0;
  uint64_t security_bytes = 0;
  uint64_t repeated_bytes = 0;
  uint64_t unknown_bytes = 0;

  while(!std::feof(f)) {
    
    // feof() only protects when we read past end of file, so check first
    uint64_t offset = ftell(f);
    if (offset == file_length) {
      break;
    }

    // Read one sector
    uint32_t sector = offset / 2048LLU;
    uint32_t data_area_sector = (offset - data_area_offset) / 2048LLU;
    std::printf("at offset: %" PRIu64 "; sector %" PRIu32 "; ", offset, sector);
    if (offset >= data_area_offset) {
      std::printf("data area sector: %" PRIu32 "; ", data_area_sector);
    }
    size_t read_bytes = std::fread(sector_buffer, 1, 2048, f);

    // We always need 2048 bytes, or we get lost
    if (read_bytes != 2048) {
      std::fprintf(stderr, "Only read %d / 2048 bytes at offset: %" PRIu64 "; sector %" PRIu32, read_bytes, offset, sector);
      break;
    }

    // Check for security sectors
    struct {
      uint32_t start;
      uint32_t end;
    } security_sectors[16] = {
      { 290690, 294785 },
      { 444220, 448315 },
      { 602044, 606139 },
      { 834240, 838335 },
      { 1142070, 1146165 },
      { 1301816, 1305911 },
      { 1451028, 1455123 },
      { 1607712, 1611807 },
      { 1985176, 1989271 },
      { 2289892, 2293987 },
      { 2448988, 2453083 },
      { 2602436, 2606531 },
      { 2755260, 2759355 },
      { 2915402, 2919497 },
      { 3066980, 3071075 },
      { 3295582, 3299677 }
    };
    bool is_security = false;
    for(auto& security_sector : security_sectors) {
      if (sector >= security_sector.start && sector <= security_sector.end) {
        is_security = true;
        break;
      }
    }
    if (is_security) {
      std::printf("security\n");
      size_t written = rnd.Fill(random_buffer, 2048, false);
      assert(written == 2048);
      security_bytes += 2048;
      continue;
    }

    // Check if it's a constant sector
    uint8_t const_buffer[2048];
    memset(const_buffer, sector_buffer[0], 2048);
    if (std::memcmp(sector_buffer, const_buffer, 2048) == 0) {
      std::printf("repeated 0x%02X!\n", const_buffer[0]);
      repeated_bytes += 2048;
      continue;
    }

    // Check if this sector is random data
    if (std::memcmp(sector_buffer, random_buffer, 2048) == 0) {
      std::printf("random\n");

      // Generate new random stream
      size_t written = rnd.Fill(random_buffer, 2048, false);
      assert(written == 2048);
      random_bytes += 2048;
      continue;
    }

    std::printf("unknown\n");
    unknown_bytes += 2048;

#if 0
    // Output the current bytes of the random stream
    for(unsigned int i = 0; i < 10; i++) {
      std::printf("%02X", random_buffer[i]);
      if ((i + 1) % 32 == 0) {
        std::printf("\n");
      }
    }
    std::printf("\n");
#endif

  }

  printf("\n");
  uint64_t total_bytes = 0;

  printf("Data-Area offset: %" PRIu64 " bytes\n", data_area_offset);
  total_bytes += data_area_offset;

  printf("Random: %" PRIu64 " bytes (+%" PRIu64 " bytes security)\n", random_bytes, security_bytes);
  total_bytes += random_bytes + security_bytes;

  printf("Repeated: %" PRIu64 " bytes\n", repeated_bytes);
  total_bytes += repeated_bytes;

  printf("Unknown: %" PRIu64 " bytes\n", unknown_bytes);
  total_bytes += unknown_bytes;

  printf("\n");
  printf("Total: %" PRIu64 " bytes / %" PRIu64 " bytes\n", total_bytes, file_length);

  return 0;
}
