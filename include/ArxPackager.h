// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

// Boost headers for file parsing
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>


namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

struct Item;

typedef boost::variant<boost::recursive_wrapper<Item>, std::string> SubItem;

struct Item {
    std::string name;
    //std::vector<std::string> files;
    std::vector<SubItem> subItems;
};

std::ostream& operator<<(std::ostream& os, const Item obj);

BOOST_FUSION_ADAPT_STRUCT(
	Item,
	(std::string, name)
	(std::vector<SubItem>, subItems)
)//(std::vector<std::string>, files)

template <typename Iterator>
struct ManifestGrammar : qi::grammar<Iterator, Item(), qi::ascii::space_type> {
	ManifestGrammar() : ManifestGrammar::base_type(Folder) {
        using namespace qi::labels;
		using qi::lexeme;
        using phoenix::at_c;
        using phoenix::push_back;

        Ident %= +(qi::ascii::alnum | qi::char_('_'));
        FileName %= Ident >> qi::char_('.') >> +(qi::ascii::alnum);
        Path = *(Ident >> '/')[_val += (_1 + '/')] >> FileName[_val += _1];
        Folder = Ident[at_c<0>(_val) = _1]
                >> '{' >> *qi::ascii::space
                >> +subItem[push_back(at_c<1>(_val), _1)]
                >> -qi::ascii::space
                >> '}';
       subItem %= Path | Folder; //[at_c<0>(_val) = _1]
	}
	qi::rule<Iterator, std::string()> Ident;
	qi::rule<Iterator, std::string()> FileName;
	qi::rule<Iterator, std::string(), qi::ascii::space_type> Path;
	qi::rule<Iterator, SubItem(), qi::ascii::space_type> subItem;
    qi::rule<Iterator, Item(), qi::ascii::space_type> Folder;
};


struct Folder {
	Folder() : name("") {}
	Folder(Item const item);

	std::string name;
	std::vector<std::string> files;
	std::vector<Folder> subfolders;
};

struct SubItemSplitter : boost::static_visitor<> {
	Folder * const parentFolder;

	SubItemSplitter(Folder * const folder) : parentFolder(folder) {}
	void operator()(Item const &item);
	void operator()(Item const &item) const;
	void operator()(std::string filepath);
	void operator()(std::string filepath) const;
};

class ArxPackager {
    std::ofstream arx;
    std::ifstream manifest;
    Folder buildTree();
public:
    ArxPackager(const char *arxPath, const char *manifestPath);
    ~ArxPackager();

    //void build();
};
