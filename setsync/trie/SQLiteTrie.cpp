/*
 * SQLiteTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SQLiteTrie.h"

namespace trie {

SQLiteTrie::SQLiteTrie(const utils::CryptoHash& hash): Trie(hash){

}

SQLiteTrie::~SQLiteTrie(){

}

bool SQLiteTrie::add(const unsigned char * hash){

}
bool SQLiteTrie::add(const unsigned char * hash, bool performhash){

}
bool SQLiteTrie::remove(const unsigned char * hash){

}
bool SQLiteTrie::remove(const unsigned char * hash, bool performhash){

}
bool SQLiteTrie::contains(const unsigned char * hash) const{
	return false;
}
void SQLiteTrie::clear(void){

}

bool SQLiteTrie::operator ==(const Trie& other) const{
	//TODO
	return false;
}
}
