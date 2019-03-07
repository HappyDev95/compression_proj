# compression_proj

A lossless compression implementation to be used on bitsets

compress.hpp
  -contains the implementation
  -provided compress and decompress methods as well as associated methods to aid in creating a huffman tree
  -In compress method the huffman tree is encoded into the output bitset
  -In decompress method the huffmantree that was encoded is decoded to recreate the original bitset that was to be encoded
  
compress.cpp
  -reads in a file, 'data.bin', that contains the information to be compressed
  -creates a bitset and then calls the compress and decompress methods provided in compress.hpp
  -makes sure that these two bitsets are equivalent (the compression is lossless) at the end
  
