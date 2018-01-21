// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ArxPackager.h"

std::ostream& operator<<(std::ostream& os, const Item obj) {
	os << obj.name << std::endl;
	for (SubItem item : obj.subItems) {
		os << item << std::endl;
	}
	return os;
}

//SubItemSplitter::SubItemSplitter(Folder * const folder) {
//	parentFolder = folder;
//}

void SubItemSplitter::operator()(Item const &item) {
	parentFolder->subfolders.emplace_back(item);
}

void SubItemSplitter::operator()(Item const &item) const {
	parentFolder->subfolders.emplace_back(item);
}

void SubItemSplitter::operator()(std::string filepath) {
	parentFolder->files.emplace_back(filepath);
}

void SubItemSplitter::operator()(std::string filepath) const {
	parentFolder->files.emplace_back(filepath);
}

Folder::Folder(Item const item) {
	name = item.name;
	for (SubItem subitem : item.subItems) {
		boost::apply_visitor(SubItemSplitter(this), subitem);
	}
}

Folder ArxPackager::buildTree(){
    using qi::parse;
	using qi::phrase_parse;
	using phoenix::push_back;

	manifest.seekg(0, std::ios::end);
	size_t size = manifest.tellg();
	manifest.seekg(0);
	std::string manBuffer(size, ' ');
	manifest.read(&manBuffer[0], size);
	
	ManifestGrammar<std::string::const_iterator> grammar;
	Item root;
	std::cout << phrase_parse(manBuffer.begin(), manBuffer.end(), grammar, qi::ascii::space, root) << std::endl;
	std::cout << root.name << root.subItems.size() << std::endl;
	std::cout << root;


	Folder rootFolder(root);
    return rootFolder;
}

ArxPackager::ArxPackager(const char *arxPath, const char *manifestPath){
    manifest = std::ifstream(manifestPath, std::ios::in);

    arx = std::ofstream(arxPath, std::ios::out | std::ios::binary);

    Folder root = buildTree();
	std::cout << root.name << ' ' << root.files.size() << ' ' << root.subfolders.size() << std::endl;
}

ArxPackager::~ArxPackager(){
    manifest.close();
    arx.close();
}
