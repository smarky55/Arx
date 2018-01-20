// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ArxPackager.h"

Folder ArxPackager::buildTree(){
    Folder rootFolder("");

    using qi::parse;
	using phoenix::push_back;

	/*qi::rule<std::string> ident = *(qi::alnum | qi::char_("_"));
	qi::rule<std::string> fileName = ident >> "." >> *qi::alnum;
	qi::rule<std::string> path = *(ident >> "/") >> fileName;
	qi::rule<std::string> folder;
	folder = ident >> "{" >> *(path | folder) >> "}";*/

    return rootFolder;
}

ArxPackager::ArxPackager(const char *arxPath, const char *manifestPath){
    manifest = std::ifstream(manifestPath, std::ios::in);

    arx = std::ofstream(arxPath, std::ios::out | std::ios::binary);

    Folder root = buildTree();
}

ArxPackager::~ArxPackager(){
    manifest.close();
    arx.close();
}
