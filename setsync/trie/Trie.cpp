/*
 * Trie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Trie.h"
#ifdef HAVE_OPENSSL_SHA_H
#include <openssl/sha.h>
#else
#define SHA_DIGEST_LENGTH 20
#include "setsync/sha1.h"
#endif

namespace trie {

Trie::Trie(const size_t hashsize) : hashsize_(hashsize){
	this->size = 0;
	this->hashscratch = new unsigned char[this->hashsize_*2];
}

Trie::~Trie() {
}

size_t Trie::getSize() const{
	return this->size;
}

void Trie::setHashSize(const size_t size){
	this->hashsize_ = size;
}

size_t Trie::getHashSize() const{
	return this->hashsize_;
}

void Trie::setSize(const size_t size){
	this->size = size;
}

void Trie::incSize(){
	this->size++;
}

void Trie::decSize(){
	this->size--;
}

bool Trie::isHashPerformingNedded(){
	return this->needsHashPerforming;
}

void Trie::setHashingPerformed(){
	this->needsHashPerforming = false;
}

}
