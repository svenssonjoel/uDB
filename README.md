
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
