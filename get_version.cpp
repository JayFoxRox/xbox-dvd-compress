#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>

int main(int argc, char* argv[]) {

  FILE* f = fopen(argv[1], "rb");

  fseek(f, 0x18310000LLU + 2048LLU, SEEK_SET);
  
  uint8_t sector[2048];
  fread(sector, 2048, 1, f);
  
  uint8_t magic[32] = {
    0x58, 0x42, 0x4F, 0x58, 0x5F, 0x44, 0x56, 0x44,
    0x5F, 0x4C, 0x41, 0x59, 0x4F, 0x55, 0x54, 0x5F,
    0x54, 0x4F, 0x4F, 0x4C, 0x5F, 0x53, 0x49, 0x47,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  struct Version {
    uint16_t major;
    uint16_t minor;
    uint16_t build;
    uint16_t qfe;
  };

  if (memcmp(&sector[0], magic, 32) != 0) {
    std::printf("Magic bad!\n");
  }

  unsigned int version_count = 0;
  unsigned int expected_version_count = 0;

  uint8_t zero[2048];
  memset(zero, 0x00, sizeof(zero));

  if (memcmp(&sector[32], zero, 8) == 0) {
    Version* xbgamedisc_version = reinterpret_cast<Version*>(&sector[48]);
    std::printf("xbgamedisc version: %d.%d.%d.%d.%d\n", xbgamedisc_version->major >> 8,
                                                        xbgamedisc_version->major & 0xFF,
                                                        xbgamedisc_version->minor,
                                                        xbgamedisc_version->build,
                                                        xbgamedisc_version->qfe);
    version_count++;

    if (memcmp(&sector[72], zero, 8) != 0) {
      // I'm guessing this is a mastering tool. xbgamedisc did not fill the
      // random sectors anymore, so MS must have had a new tool to fill these.
      Version* unknown_version = reinterpret_cast<Version*>(&sector[72]);
      std::printf("mastering tool version: %d.%d.%d.%d.%d\n", unknown_version->major >> 8,
                                                              unknown_version->major & 0xFF,
                                                              unknown_version->minor,
                                                              unknown_version->build,
                                                              unknown_version->qfe);
      version_count++;
    }
  } else {
    Version* xblayout_version = reinterpret_cast<Version*>(&sector[32]);
    std::printf("xblayout version: %d.%d.%d.%d\n", xblayout_version->major,
                                                   xblayout_version->minor,
                                                   xblayout_version->build,
                                                   xblayout_version->qfe);
    version_count++;

    if (xblayout_version->build < 4721) {
      Version* xbpremaster_version = reinterpret_cast<Version*>(&sector[40]);
      std::printf("xbpremaster version: %d.%d.%d.%d\n", xbpremaster_version->major,
                                                        xbpremaster_version->minor,
                                                        xbpremaster_version->build,
                                                        xbpremaster_version->qfe);
      version_count++;
    }
  }

  for(unsigned int i = 32; i < 2048; i += 8) {
    Version* version = reinterpret_cast<Version*>(&sector[i]);
    if (memcmp(version, zero, 8) == 0) {
      continue;
    }
    expected_version_count++;
#if 0
    std::printf("Version [offset %d]: %d.%d.%d.%d (or maybe %d.%d.%d.%d.%d)\n", i, version->major,
                                                                                   version->minor,
                                                                                   version->build,
                                                                                   version->qfe,
                                                                                   version->major >> 8,
                                                                                   version->major & 0xFF,
                                                                                   version->minor,
                                                                                   version->build,
                                                                                   version->qfe);
#endif
  }

  fclose(f);

  if (version_count != expected_version_count) {
    std::fprintf(stderr, "Version printing broken!\n");
  }

  return 0;
}
