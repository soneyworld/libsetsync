/*
 * Trie.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Trie.h"
namespace setsync {
namespace trie {

Trie::Trie(const crypto::CryptoHash& hash) :
	hash_(hash) {
	this->size = 0;
	this->hashscratch = new unsigned char[this->hash_.getHashSize() * 2];
}

Trie::~Trie() {
	delete[] this->hashscratch;
}

size_t Trie::getSize() const {
	return this->size;
}

const crypto::CryptoHash& Trie::getHash() const {
	return this->hash_;
}

void Trie::setSize(const size_t size) {
	this->size = size;
}

void Trie::incSize() {
	this->size++;
}

void Trie::decSize() {
	this->size--;
}

bool Trie::isHashPerformingNedded() {
	return this->needsHashPerforming;
}

void Trie::setHashingPerformed() {
	this->needsHashPerforming = false;
}

bool Trie::add(const unsigned char * hash) {
	return add(hash, true);
}

bool Trie::remove(const unsigned char * hash) {
	return remove(hash, true);
}

bool Trie::remove(const char * str) {
	return remove(str, true);
}

bool Trie::remove(const char * str, bool performhash) {
	unsigned char c[this->hash_.getHashSize()];
	this->hash_(c, str);
	return remove(c, performhash);
}

bool Trie::remove(const std::string& str) {
	return remove(str, true);
}

bool Trie::remove(const std::string& str, bool performhash) {
	unsigned char c[this->hash_.getHashSize()];
	this->hash_(c, str);
	return remove(c, performhash);
}

bool Trie::add(const char * str, bool performhash) {
	unsigned char c[this->hash_.getHashSize()];
	this->hash_(c, str);
	return add(c, performhash);
}

bool Trie::add(const std::string& str, bool performhash) {
	unsigned char c[this->hash_.getHashSize()];
	this->hash_(c, str);
	return add(c, performhash);
}

bool Trie::add(const char * str) {
	return add(str, true);
}

bool Trie::add(const std::string& str) {
	return add(str, true);
}

TrieNodeType Trie::contains(const char * str) const {
	unsigned char c[this->hash_.getHashSize()];
	this->hash_(c, str);
	return contains(c);
}

TrieNodeType Trie::contains(const std::string& str) const {
	unsigned char c[this->hash_.getHashSize()];
	this->hash_(c, str);
	return contains(c);
}

std::string Trie::toDotString() const {
	return toDotString("N");
}
}
}
