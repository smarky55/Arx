// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.


#include "ArxPackager.h"

int main(int argc, char *argv[]){
    if(argc == 3){
        Arx::ArxPackager packager(argv[2], argv[1]);
	}
	else {
		std::cout << "Usage: manifest_path arx_path" << std::endl;
	}

    return 0;
}
