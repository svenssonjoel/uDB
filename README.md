
# uDB - MicroDatabase

 Flash database targeting NAND flash architectures.
 

 NAND:
 1. Smallest write-size is larger than a byte (2048 Bytes for example).
 2. Smallest read-size is larger than a byte (2048 bytes for example).
 3. Smallest erasable unit is a block, perhaps 128K  as an example (64 pages).
 4. A block can be written once per erase.


 Page  : smallest programmable unit (ex: 2048Byte)
 Block : smallest erasable unit (ex 128K) 