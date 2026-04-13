#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../src/udb.h"

#define SECTOR_SIZE   4096
#define NUM_SECTORS   4
#define FLASH_SIZE    (SECTOR_SIZE * NUM_SECTORS)

static uint8_t flash_mem[FLASH_SIZE];

static bool emu_read(uint32_t addr, void *buf, size_t len) {
  if (addr + len > FLASH_SIZE) return false;
  memcpy(buf, &flash_mem[addr], len);
  return true;
}

static bool emu_write(uint32_t addr, const void *buf, size_t len) {
  if (addr + len > FLASH_SIZE) return false;
  for (size_t i = 0; i < len; i++) {
    flash_mem[addr + i] &= ((uint8_t *)buf)[i];
  }
  return true;
}

static bool emu_erase(uint32_t addr, size_t len) {
  if (addr + len > FLASH_SIZE) return false;
  memset(&flash_mem[addr], 0xFF, len);
  return true;
}

// add a real one ...
static uint16_t emu_crc16(const void *buf, size_t len) {
  (void) buf;
  (void) len;
  return 42;
}

static void flash_init(void) {
  memset(flash_mem, 0xFF, FLASH_SIZE);
}

#define TEST(name)                              \
  do {                                          \
    printf("  %-40s ", #name);                  \
    if (name()) printf("PASS\n");               \
    else        printf("FAIL\n");               \
  } while(0)

static uint32_t base_addresses[NUM_SECTORS];

static udb_hal_t make_hal(void) {
  for (int i = 0; i < NUM_SECTORS; i++) {
    base_addresses[i] = i * SECTOR_SIZE;
  }
  udb_hal_t hal = {
    .base_addresses  = base_addresses,
    .num_sectors     = NUM_SECTORS,
    .sector_size     = SECTOR_SIZE,
    .min_write_size  = 1,
    .read            = emu_read,
    .write           = emu_write,
    .erase           = emu_erase,
    .crc16           = emu_crc16,
  };
  return hal;
}

static bool test_init_succeeds(void) {
  flash_init();
  udb_hal_t hal = make_hal();
  udb_t udb;
  return udb_init(&udb, &hal);
}

static bool test_init_fails_null_1(void) {
  udb_t udb;
  return !udb_init(&udb, NULL);
}

static bool test_init_fails_null_2(void) {
  udb_hal_t hal;
  return !udb_init(NULL, &hal);
}

static bool test_init_fails_too_few_sectors(void) {
  flash_init();
  udb_hal_t hal = make_hal();
  udb_t udb;
  hal.num_sectors = 1;
  return !udb_init(&udb, &hal);
}

static bool test_put_get_1(void) {
  flash_init();
  udb_hal_t hal = make_hal();
  udb_t udb;

  if (udb_init(&udb, &hal)) {
    uint8_t pl[8] = {0, 1, 2, 3,
                     4, 5, 6, 7};
    if (udb_put(&udb, 1, pl, 8)) {
      uint8_t rl[8] = {0};

      udb_get(&udb, 1, rl, 8);

      bool ok = true;
      for (int i = 0; i < 8; i ++) {
        if (rl[i] != pl[i]) ok = false;
      }
      return ok;
    } else {
      return false;
    }
    
  }
  return false;
}

int main(void) {
  printf("uDB tests\n");
  TEST(test_init_succeeds);
  TEST(test_init_fails_null_1);
  TEST(test_init_fails_null_2);
  TEST(test_init_fails_too_few_sectors);
  TEST(test_put_get_1);
  return 0;
}
