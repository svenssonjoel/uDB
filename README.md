
# uDB - MicroDatabase

  Goals:
  1. Small, memory efficient and platform portable database system for
     microcontrollers with some kind of flash storage.
  2. Small ram footprint and small easily integrateable code (a c/h pair)
  3. A simple interface
  4. Simple overlookable code!
     Avoid feature creep!

  Plan:
  1. uDB will operate on set of flash sectors that are handed to udb
     at the initialization.
  2. The flash sectors given to uDB will all be the same size.
  3. uDB owns the entire sector (cannot be sheared with other uses).
  4. At least 2 sectors must be given to uDB.
  5. DB Compaction will be performed incrementally in small steps via
     a udb_compaction_step function called from the integrator application.
  6. DB writes, queries and compaction will be done from the same
     OS thread.
  7. queries should go through even if the DB is in the compaction state.
  8. Writes are blocked during compaction.

  9. The uDB assigns no semantics to the keys or values.
     - The key is just a 32 bit value and
     - the value is just a blob.

  10. Queries:
      - key lookups.
      - iterator using bitmask lookups. (for example time series)

  Tricky bits:
  1. Handling cases where the mcu dies mid write or mid compaction.
     - The book "Operating Systems - Three easy pieces" chapter 43
       "log-structured file systems" will be read for inspiration.
  2. Efficiency (in space and time)


# API

## Configuration

The `udb_hal_t` is used to configure an instance of uDB.

```
typedef struct {
  uint32_t *base_addresses;
  uint32_t num_sectors;
  uint32_t sector_size;
  uint32_t min_write_size;

  // low level operations on the flash memory
  bool (*read)(uint32_t addr, void *buf, size_t len);
  bool (*write)(uint32_t addr, const void *buf, size_t len);
  bool (*erase)(uint32_t addr, size_t len);
  // Utility functions provided by target platform
  uint16_t (*crc16)(const void *buf, size_t len);
} udb_hal_t;
```

`base_addresses` is an array of `num_sectors` addresses of flash
sectors. All sectors must be the same size, `sector_size`

TBD: min_write_size will most likely be required to be a single byte,
     at least to begin with.

`read`, `write` and `erase` are callbacks provided by the user of uDB
for reading, writing and erasing flash.

`crc16` is a user provided crcr16 implementation. Most embedded
software will very likely already have an implementation of crc16 so
this is to be able to reuse that code.

## initialization

```
bool udb_init(udb_t *udb, udb_hal_t *hal);
```

The init function takes a pointer to a `udb_hal_t` configuration
and uses the values in that configuration to initialize the uDB.
The `udb_t` represents the state of the instance of uDB that we initialize
and is populated but the init function.

## Get and Put

TBD exact interface but `udb_put` will store a new, or update an old, KV pair
in the database. Since this is flash, update means to "invalidate" the old occurrency
and then write a new fresh KV later in memory.

Compaction (GC) will clean up invalidated entries periodically.

## Iterator API

TBD exact interface but this will be an api for initializing  "iterator"
objects. An udb_iterate function or similar, will take the iterator and give
a value until there are no more values to give.
The iterator should support some simple predicates. something like
bitwise-pattern-matching.

Example, give me all values that have a key where the highest three bits are 101





