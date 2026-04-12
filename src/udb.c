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

#include "udb.h"
#ifdef DEBUG
#include <stdio.h>
#define DBGPRINT printf
#else
#define DBGPRINT
#endif

static const uint16_t UDB_MAGIC = 0xDBDB;

typedef enum {
  UDB_SECTOR_STATUS_ERASED = 0xFF,
  UDB_SECTOR_STATUS_ACTIVE = 0xFE,
  UDB_SECTOR_STATUS_COMPACTING = 0xFC,
  UDB_SECTOR_STATUS_GARBAGE = 0xF8
} udb_sector_status_t;

// The sequence number is there to create an ordering
// of operations to flash sectors. The idea is that
// every erase will assign a new sequence number to the sector.
// With this we will know that the sector with the highest sequence
// number was most recently cleared.
//
// By preferring the ERASED sector with the smallest sequence number as the next write target
// one should guarantee wear leveling (round robin usage).
//
// The counter can also be used to find the "most likely" active for writing
// sector upon a restart of the system. the Sector that is ACTIVE and has
// the highest sequence number of all ACTIVE sectors is the one to write to.
//
// Over a small set of sectors a 32bit number will be able to count
// way more erase cycles than any flash can handly (is this correct) ?
typedef struct __attribute__((packed)) {
  uint16_t magic;
  uint8_t status;
  uint32_t sequence;
} udb_sector_header_t;

static bool get_sector_header(udb_t *udb, uint32_t sector, udb_sector_header_t *header) {
  return udb->hal.read(udb->hal.base_addresses[sector], header, sizeof(udb_sector_header_t));
}

// Initialize the DB and find the active sector
//  1. No sector has the magic indicator.
//  2. There are sectors with magic:
//     a: A sector with magic has status ACTIVE.
//     b: No sector with magic har status ACTIVE.
//  
bool udb_init(udb_t *udb, udb_hal_t *hal) {
  bool r = false;
  if (udb && hal) {
    udb->hal = *hal;

    udb->counter = 0;
    if (udb->hal.num_sectors < 2) goto init_done;

    // recover counter
    for (uint32_t i = 0; i < udb->hal.num_sectors; i ++) {
      udb_sector_header_t header;
      get_sector_header(udb, i, &header);
      DBGPRINT("0x%x\n", header.magic);
      DBGPRINT("0x%x\n", header.status);
      DBGPRINT("0x%x\n", header.sequence);
      if (header.magic == UDB_MAGIC) {
        if (header.status == UDB_SECTOR_STATUS_ACTIVE ||
            header.status == UDB_SECTOR_STATUS_COMPACTING) {
           if (header.sequence >= udb->counter) {
            udb->counter = header.sequence;
          }
        }
      }
    }
    udb->counter++; 
    // Counter is recovered
    // Initialize non-initialized sectors and
    // recover garbage left by a untimely powercycle.
    for (uint32_t i = 0; i < udb->hal.num_sectors; i ++) {
      udb_sector_header_t header;
      get_sector_header(udb, i, &header);
      DBGPRINT("0x%x\n", header.magic);
      DBGPRINT("0x%x\n", header.status);
      DBGPRINT("0x%x\n", header.sequence);
      if (header.magic != UDB_MAGIC) {
        // For now, just erase the sector
        // In the future, check if it is erased already and just
        // write a new header.
        udb_sector_header_t h;
        h.magic = UDB_MAGIC;
        h.status = UDB_SECTOR_STATUS_ERASED;
        h.sequence = udb->counter++;
        hal->erase(udb->hal.base_addresses[i], hal->sector_size);
        hal->write(udb->hal.base_addresses[i], &h, sizeof(udb_sector_header_t));
      } else {
        // there is a header here
        switch (header.status) {
        case UDB_SECTOR_STATUS_ERASED:
          // Ignore.
          break;
        case UDB_SECTOR_STATUS_ACTIVE:
          // ignore.
          break;
        case UDB_SECTOR_STATUS_COMPACTING:
          break;
        case UDB_SECTOR_STATUS_GARBAGE:
          // If a GARBAGE sector is found it can be erased
          // and returned to ERASED state here
          // The sector state is set to GARBAGE after all live
          // values have been moved out of it. so removing garbage
          // here is perfectly safe.
          DBGPRINT("Sector in state GARBAGE fount at init\n");
          udb_sector_header_t h;
          h.magic = UDB_MAGIC;
          h.status = UDB_SECTOR_STATUS_ERASED;
          h.sequence = udb->counter++;
          hal->erase(udb->hal.base_addresses[i], hal->sector_size);
          hal->write(udb->hal.base_addresses[i], &h, sizeof(udb_sector_header_t));
          break;
        }
      }
    }
    r = true;
  }
 init_done:
  return r;
}

bool udb_put(udb_t *udb, uint32_t key, uint8_t *payload, size_t size) {
  (void) udb;
  (void) key;
  (void) payload;
  (void) size;
  // use the active sector and the write_pos.
  // payload must be smaller than a (sector_size - sector headers entry headers and so on.)
  return true;
}

// Read sector headers

// Compaction

// Clearing

// Recovery

// 



