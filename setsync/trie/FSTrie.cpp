/*
 * FSTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "FSTrie.h"

namespace trie {

FSTrie::FSTrie(const utils::CryptoHash& hash) :
	Trie(hash) {
	// TODO Auto-generated constructor stub

}

FSTrie::~FSTrie() {
	// TODO Auto-generated destructor stub
}
bool FSTrie::add(const unsigned char * hash) {
	return false;
}
bool FSTrie::add(const unsigned char * hash, bool performhash) {
	return false;
}
bool FSTrie::remove(const unsigned char * hash) {
	return false;
}
bool FSTrie::remove(const unsigned char * hash, bool performhash) {
	return false;
}

enum TrieNodeType FSTrie::contains(const unsigned char * hash) const {
	return NOT_FOUND;
}
void FSTrie::clear(void) {

}
bool FSTrie::operator ==(const Trie& other) const {
	//TODO
	return false;
}

size_t FSTrie::getSubTrie(const unsigned char * hash, void * buffer,
		const size_t buffersize) {
	//TODO
	return 0;
}
bool FSTrie::getRoot(unsigned char * hash){
	//TODO
	return false;
}
}
