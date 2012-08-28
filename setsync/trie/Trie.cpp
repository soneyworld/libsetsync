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

Trie::Trie() {
	this->size = 0;
	this->hashsize = SHA_DIGEST_LENGTH;
}

Trie::~Trie() {
}

size_t Trie::getSize(){
	return this->size;
}

void Trie::setHashSize(size_t size){
	this->hashsize = size;
}

void Trie::setSize(size_t size){
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
