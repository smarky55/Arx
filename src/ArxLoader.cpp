<<<<<<< HEAD
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
=======
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
>>>>>>> 2bbb38892a1a5653b0034c5f1fd909c91fcbd22d

#include "ArxLoader.h"


ArxLoader::ArxLoader(const char * arx_path) {
	arx = std::ifstream(arx_path, std::ios::in | std::ios::binary);

	if(!arx.is_open()) {
		throw std::runtime_error("Failed to open file: " + std::string(arx_path));
	}

	arx.read(header, 0x10);
	// Check file is arx archive
	if(memcmp(header, "ARX", 3) != 0) {
		throw std::runtime_error("Invalid archive: " + std::string(arx_path));
	}

	char major = header[3];
	char minor = header[4];
	char rev = header[5];

	if(major != 0 || minor != 0 || rev != 0) {
		throw std::runtime_error("Unsupported archive version");
	}

	unsigned baseDirLoc = *(unsigned*)&header[6];
	unsigned baseDirSize = *(unsigned*)&header[0xA];

	// std::cout << "Archive " << arx_path << " loaded. Version " << +major << "." << +minor << "." << +rev << std::endl;
}

ArxLoader::~ArxLoader() {
	arx.close();
}

void ArxLoader::getFile(const char * path, char * &data, unsigned &size, unsigned location, unsigned length) {
	std::regex regex("(?:(\\w+):)?((?:\\w+:)*\\w+\\.\\w+)");
	if(location == 0 || length == 0) { // Read from base directory
		location = *(unsigned*)&header[6];
		length = *(unsigned*)&header[0xA];
	}
	// std::cout << location << ' ' << length << std::endl;
	arx.seekg(location);
	char * record = new char[length];
	arx.read(record, length);
	if(memcmp(record, "DIR", 3) == 0) { // Find next record
		std::cmatch match;
		if(std::regex_match(path, match, regex)) {
			// std::cout << match[2] << std::endl;
			size_t i;
			for(i = 3; i < length; i += 64) {
				if(std::string(&record[i+8]) == match[1].str() || std::string(&record[i + 8]) == match[2].str()) {
					break;
				}
			}
			unsigned nextLoc = *(unsigned*)&record[i];
			unsigned nextLen = *(unsigned*)&record[i + 4];
			getFile(match[2].str().c_str(), data, size, nextLoc, nextLen);
		} else {
			throw std::runtime_error("Unable to find record" + std::string(path));
		}
	} else if(memcmp(record, "FIL", 3) == 0) { // Copy record to data buffer less the record tag
		data = new char[length-3];
		std::memcpy(data, record+3, length-3);
		size = length-3;
	} else {
		throw std::runtime_error("Unknown record met");
	}

	arx.seekg(0);
}

void ArxLoader::getStream(const char * path, std::istream * &stream) {
	char * buf;
	unsigned length;
	getFile(path, buf, length);
	stream = new std::istringstream(std::string(buf, length));
}
