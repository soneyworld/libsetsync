/*
 * FSTrie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "FSTrie.h"

namespace trie {

FSTrie::FSTrie(const utils::CryptoHash& hash) : Trie(hash) {
	// TODO Auto-generated constructor stub

}

FSTrie::~FSTrie() {
	// TODO Auto-generated destructor stub
}
bool FSTrie::add(const unsigned char * hash){
	return false;
}
bool FSTrie::add(const unsigned char * hash, bool performhash){
	return false;
}
bool FSTrie::remove(const unsigned char * hash){
	return false;
}
bool FSTrie::remove(const unsigned char * hash, bool performhash){
	return false;
}

bool FSTrie::contains(const unsigned char * hash) const{
	return false;
}
void FSTrie::clear(void){

}
bool FSTrie::operator ==(const Trie& other) const{
	//TODO
	return false;
}
}
