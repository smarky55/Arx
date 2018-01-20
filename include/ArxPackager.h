// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

struct Folder {
	Folder(std::string folderName) { name = folderName; };
    std::string name;
    std::vector<std::string> files;
    std::vector<Folder> subfolders;
};

class ArxPackager {
    std::ofstream arx;
    std::ifstream manifest;
    Folder buildTree();
public:
    ArxPackager(const char *arxPath, const char *manifestPath);
    ~ArxPackager();

    void build();
};
