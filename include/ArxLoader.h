// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include <ArxCommon.h>

namespace Arx{

	class ArxLoader {
		std::ifstream arx;
		char header[0x10];
	public:
		ArxLoader(const char * arx_path);
		~ArxLoader();

		void getFile(const char * path, char * &data, unsigned &size, unsigned location = 0, unsigned length = 0);
		void getStream(const char * path, std::istream * &stream);
	};
}
