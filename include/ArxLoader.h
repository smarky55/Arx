#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

class ArxLoader {
	std::ifstream arx;
	char header[0x10];
public:
	ArxLoader(const char * arx_path);
	~ArxLoader();

	void getFile(const char * path, char * &data, unsigned &size, unsigned location = 0, unsigned length = 0);
	void getStream(const char * path, std::istream * &stream);
};

