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

#define UDB_MAGIC 0xDBDB



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
    if (udb->hal.num_sectors < 2) goto init_done;

    for (uint32_t i = 0; i < udb->hal.num_sectors; i ++) {
      uint16_t magic;
      udb->hal.read(udb->hal.base_addresses[i], &magic, 2);
      DBGPRINT("0x%x\n", magic);
    }
    r = true;
  }
 init_done:
  return r;
}

// Read sector headers

// Compaction

// Clearing

// Recovery

// 



