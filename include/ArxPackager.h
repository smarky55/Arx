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

namespace Arx{
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
}

// Macro MUST be run in global scope, breaking up the namespace block to keep close to struct definition
BOOST_FUSION_ADAPT_STRUCT(
    Arx::Item,
    (std::string, name)
    (std::vector<Arx::SubItem>, subItems)
)

namespace Arx {
    template <typename Iterator>
    struct ManifestGrammar : qi::grammar<Iterator, std::vector<SubItem>(), qi::ascii::space_type> {
    	ManifestGrammar() : ManifestGrammar::base_type(SubItemList) {
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
		   SubItemList %= +subItem;
    	}
    	qi::rule<Iterator, std::string()> Ident;
    	qi::rule<Iterator, std::string()> FileName;
    	qi::rule<Iterator, std::string(), qi::ascii::space_type> Path;
    	qi::rule<Iterator, SubItem(), qi::ascii::space_type> subItem;
        qi::rule<Iterator, Item(), qi::ascii::space_type> Folder;
		qi::rule<Iterator, std::vector<SubItem>(), qi::ascii::space_type> SubItemList;
    };


    struct Folder {
    	Folder() : name("") {}
    	Folder(Item const item);
		Folder(std::vector<SubItem> const &SubItems);
    	size_t size() { return files.size() + subfolders.size(); };
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


    	/**
    	 * Build file tree from the loaded manifest file.
    	 * \returns The root folder of the file tree
    	 */
        Folder buildTree();

        /**
         * Template function to handle writing of data in a binary format. Primarily for
    	 * writing integers.
         * \tparam T Type of @p data to write to @p file
         * \param data Data to convert to binary format
         * \param file File stream to write to
         */
        template <typename T>
        void writeData(T data, std::ofstream &file);


    	/**
    	 * Write a number of null bytes to file.
    	 * \param num Number of bytes to write.
    	 * \param file File stream to write to.
    	 */
    	void writeNull(int num, std::ofstream &file);

    	/**
    	 * Recursive function to handle the writing of a folder to file.
    	 * \param folder The current working folder.
    	 * \param file File stream to write to.
    	 */
    	void package(Folder &folder, std::ofstream &file);

    	/**
    	 * Get the length of a file.
    	 * \param file File stream to get length of.
    	 * \returns Length of file stream.
    	 */
    	size_t getFileLength(std::ifstream &file);

    	/**
    	 * Get the length of a file.
    	 * \param file File stream to get length of.
    	 * \returns Length of file stream.
    	 */
    	size_t getFileLength(std::ofstream &file);

    	/**
    	 * Expands a file by @p amount bytes. Simplifies some operations.
    	 * \param amount Number of bytes to expand the file by.
    	 * \param file File stream to expand.
    	 */
    	void expand(size_t amount, std::ofstream &file);

    public:
        ArxPackager(const char *arxPath, const char *manifestPath);
        ~ArxPackager();

        //void build();
    };
}
