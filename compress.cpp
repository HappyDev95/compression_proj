#include "compress.hpp"
#include <cstdint>
#include <fstream>
#include <vector>
#include <boost/math/cstdfloat/cstdfloat_types.hpp>
#include <climits>
#include <cstring>
#include <iostream>
#include <boost/numeric/conversion/cast.hpp>

typedef boost::float32_t float32;
typedef boost::uint32_t uint32;

int main(void){
       std::vector<unsigned char> data;
       unsigned char *cp;
       float32 value;
       value = 8.0;
       boost::uint32_t internal;
       std::memcpy( &internal , &value , sizeof( value ) );

       std::ifstream fin("data.bin", std::ios::binary);
       while (fin.read(reinterpret_cast<char*>(&internal), sizeof(uint32))){
	      std::memcpy( &value, &internal , sizeof( internal ) );
	      cp = (unsigned char *)&internal;
      	      for(int i = 0; i < 4; ++i){
		      data.push_back(*cp);
		      ++cp;
              }
	}
	boost::dynamic_bitset<unsigned char> c_db;
        boost::dynamic_bitset<unsigned char> d_db;
        boost::dynamic_bitset<unsigned char> db, backup;
        db.init_from_block_range(data.begin(), data.end());
	backup = db;
	std::cout << db << std::endl;
	std::cout << "Input\n==============================\n";
	std::cout << db << std::endl;
	compress(db, c_db);
	std::cout << "Compressed\n==============================\n";
	std::cout << db << std::endl;
	decompress(c_db,d_db);
	assert(backup == d_db);
	return 0;
}
