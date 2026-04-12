/*
MIT License

Copyright (c) 2026 Joel Svensson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _UDB_H_
#define _UDB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
  
typedef struct {
  uint32_t *base_addresses;
  uint32_t sector_size;
  uint32_t num_sectors;
  uint32_t min_write_size;

  // low level operations on the flash memory
  bool (*read)(uint32_t addr, void *buf, size_t len);
  bool (*write)(uint32_t addr, const void *buf, size_t len);
  bool (*erase)(uint32_t addr, size_t len);
  // Utility functions provided by target platform
  uint16_t (*crc16)(const void *buf, size_t len);
} udb_hal_t;

typedef enum {
  UDB_STATE_UNINITIALIZED = 0,
  UDB_STATE_AVAILABLE,
  UDB_STATE_COMPACTING,
} udb_state_t;

typedef struct {
  udb_hal_t hal;
  udb_state_t state;
  uint32_t  active_sector;
  uint32_t  write_pos;
  uint32_t  counter;
} udb_t;

  
extern bool udb_init(udb_t *udb, udb_hal_t *hal);
#ifdef __cplusplus
}
#endif

#endif
