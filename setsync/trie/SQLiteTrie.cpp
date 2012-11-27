/*
 * SQLiteTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteTrie.h"

namespace trie {

SQLiteTrie::SQLiteTrie(const utils::CryptoHash& hash) :
	Trie(hash) {

}

SQLiteTrie::~SQLiteTrie() {

}

bool SQLiteTrie::add(const unsigned char * hash) {

}
bool SQLiteTrie::add(const unsigned char * hash, bool performhash) {

}
bool SQLiteTrie::remove(const unsigned char * hash) {

}
bool SQLiteTrie::remove(const unsigned char * hash, bool performhash) {

}
TrieNodeType SQLiteTrie::contains(const unsigned char * hash) const {
	return NOT_FOUND;
}
void SQLiteTrie::clear(void) {

}

bool SQLiteTrie::operator ==(const Trie& other) const {
	//TODO
	return false;
}
size_t SQLiteTrie::getSubTrie(const unsigned char * hash, void * buffer,
		const size_t buffersize) {
	//TODO
	return 0;
}
bool SQLiteTrie::getRoot(unsigned char * hash){
	//TODO
	return false;
}
std::string SQLiteTrie::toDotString() const {
	return "";
}
}
