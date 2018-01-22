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

template <typename T>
void ArxPackager::writeData(T data, std::ofstream &file){
	file.write((char*)&data, sizeof(T));
}

void ArxPackager::writeNull(int num, std::ofstream & file){
	std::string nullstring(num, '\00');
	file.write(nullstring.data(), num);
}

void ArxPackager::package(Folder & folder, std::ofstream & file){
	file << "DIR";
	size_t length = folder.subfolders.size() + folder.files.size();
	size_t p1 = file.tellp();
	expand(64 * length, file);

	for (std::string path : folder.files) {
		std::ifstream item(path, std::ios::in | std::ios::binary);
		if (item.is_open()) {
			writeData((int)getFileLength(file), file);
			writeData((int)getFileLength(item) + 3, file);
			file << path;
			// TODO: Pad to full size & write file to archive.
		}
		else {
			throw std::runtime_error("Unable to open file: " + path);
		}
		
	}
}

size_t ArxPackager::getFileLength(std::ifstream &file) {
	size_t p = file.tellg();
	file.seekg(0, std::ios::end);
	size_t end = file.tellg();
	file.seekg(p);
	return end;
}

size_t ArxPackager::getFileLength(std::ofstream &file) {
	size_t p = file.tellp();
	file.seekp(0, std::ios::end);
	size_t end = file.tellp();
	file.seekp(p);
	return end;
}

void ArxPackager::expand(size_t amount, std::ofstream & file) {
	size_t p = file.tellp();
	file.seekp(amount - 1, std::ios::cur);
	file.write("\00", 1);
	file.seekp(p);
}

ArxPackager::ArxPackager(const char *arxPath, const char *manifestPath){
    manifest = std::ifstream(manifestPath, std::ios::in);

    arx = std::ofstream(arxPath, std::ios::out | std::ios::binary);

    Folder root = buildTree();
	std::cout << root.name << ' ' << root.files.size() << ' ' << root.subfolders.size() << std::endl;

	arx << "ARX"; // "Magic number" to check file type
	writeNull(3, arx); // Three null bytes as placeholders
	int length = root.subfolders.size() + root.files.size();
	writeData(0x10, arx); // Location of root directory
	writeData(length * 64 + 3, arx); // Length of root directory
	arx.seekp(0x10);
	package(root, arx);
}

ArxPackager::~ArxPackager(){
    manifest.close();
    arx.close();
}
