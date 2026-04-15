
# uDB - MicroDatabase

 Flash database targeting NAND flash architectures.
 

 NAND:
 1. Smallest write-size is larger than a byte (2048 Bytes for example).
 2. Smallest read-size is larger than a byte (2048 bytes for example).
 3. Smallest erasable unit is a block, perhaps 128K  as an example (64 pages).
 4. A block can be written once per erase.
 5. RAM buffer becomes essential for read and write operations.
 6. Need to keep track of broken pages.

 Page  : smallest programmable unit (ex: 2048Byte)
 Block : smallest erasable unit (ex 128K)

 ECC status reported per read.

 The protocol for knowing if a page is completely and correctly written:
  -  The page will contain a CRC32 checksum at the end.
  -  If a page is not written correctly the crc32 will fail and no data stored
     in that page can be trusted.
  
 Metadata can be stored in page0 of each block:
  - times_erased.
  - sequence number (for wear leveling)
  - state ?
  - CRC32
 Metadata is written after each erase.

 Pragmatic wear-leveling:
   - idea1: upon startup the working blocks are ordered in a sequence used
     as the "next to be cleared and relocated" order.
   - idea2: Store a sequence number somewhere (page 0)

 Incremental compaction:
   - How does compaction work on NAND?
   - Use last page as compacting flag

 Block Usage for N block memory (0 indexed)
   page 0 - metadata followed by KV
        - sequence number
   page 1 - (N-2) - KV Store
   page (N-1) - Compacting flag
        - [COMPACTING | TARGET_BLOCK | CRC]
   
   
 