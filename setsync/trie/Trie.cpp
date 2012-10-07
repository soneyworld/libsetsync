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

bool Trie::add(const unsigned char * hash){
	return add(hash, true);
}

bool Trie::remove(const unsigned char * hash){
	return remove(hash, true);
}

bool Trie::remove(const char * str){
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) str, strlen(str), c);
	return remove(c, true);
}

bool Trie::remove(const std::string& str){
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) str.c_str(), str.size(), c);
	return remove(c, true);
}

bool Trie::add(const char * str){
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) str, strlen(str), c);
	return add(c, true);
}

bool Trie::add(const std::string& str){
	unsigned char c[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*) str.c_str(), str.size(), c);
	return add(c, true);
}

}
