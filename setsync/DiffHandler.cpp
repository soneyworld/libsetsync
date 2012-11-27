/*
 * DiffHandler.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DiffHandler.h"
#include <stdlib.h>
#include <string.h>
#include <setsync/utils/OutputFunctions.h>

namespace setsync {

ListDiffHandler::ListDiffHandler() {
}

void AbstractDiffHandler::operator()(const unsigned char * hash,
		const std::size_t hashsize, const bool existsLocally) {
	this->handle(hash, hashsize, existsLocally);
}
void ListDiffHandler::handle(const unsigned char * hash,
		const std::size_t hashsize, const bool existsLocally) {
	std::vector<std::pair<unsigned char *, bool> >::iterator iter;
	for (iter = this->list_.begin(); iter != this->list_.end(); iter++) {
		if (memcmp((*iter).first, hash, hashsize) == 0) {
			// skip duplicated entry
			return;
		}
	}
	unsigned char * h = (unsigned char*) malloc(hashsize);
	memcpy(h, hash, hashsize);
	std::pair<unsigned char *, bool> entry;
	entry.first = h;
	entry.second = existsLocally;
	this->list_.push_back(entry);
}

ListDiffHandler::~ListDiffHandler() {
	std::vector<std::pair<unsigned char *, bool> >::iterator iter;
	for (iter = this->list_.begin(); iter != this->list_.end(); iter++) {
		free((*iter).first);
	}
}

const std::pair<unsigned char *, bool> ListDiffHandler::operator[](
		unsigned int const& index) const {
	if (index >= this->list_.size()) {
		std::pair<unsigned char *, bool> p;
		p.first = NULL;
		p.second = false;
		return p;
	}
	return this->list_[index];
}

const std::size_t ListDiffHandler::size() {
	return this->list_.size();
}

void ListDiffHandler::clear() {
	std::vector<std::pair<unsigned char *, bool> >::iterator iter;
	for (iter = this->list_.begin(); iter != this->list_.end(); iter++) {
		free((*iter).first);
	}
	this->list_.clear();
}

void OutputStreamDiffHandler::handle(const unsigned char * hash,
		const std::size_t hashsize, const bool existsLocally) {
	this->out_ << utils::OutputFunctions::CryptoHashtoString(hash, hashsize);
}

OutputStreamDiffHandler::OutputStreamDiffHandler(std::ostream& out) :
	out_(out) {
}

OutputStreamDiffHandler::~OutputStreamDiffHandler() {
}

C_DiffHandler::C_DiffHandler(diff_callback *callback, void *closure) :
	closure_(closure), callback_(callback) {
}

C_DiffHandler::~C_DiffHandler() {
}

void C_DiffHandler::handle(const unsigned char * hash,
		const std::size_t hashsize, const bool existsLocally) {
	(this->callback_)(this->closure_, hash, hashsize, existsLocally);
}

void UniqueFilterDiffHandler::handle(const unsigned char * hash,
		const std::size_t hashsize, const bool existsLocally) {
	std::vector<unsigned char *>::iterator iter;
	for (iter = this->list_.begin(); iter != this->list_.end(); iter++) {
		if (memcmp(*iter, hash, hashsize) == 0) {
			// skip duplicated entry
			return;
		}
	}
	unsigned char * h = (unsigned char*) malloc(hashsize);
	memcpy(h, hash, hashsize);
	this->list_.push_back(h);
	C_DiffHandler::handle(hash, hashsize, existsLocally);
}

UniqueFilterDiffHandler::~UniqueFilterDiffHandler() {
	std::vector<unsigned char *>::iterator iter;
	for (iter = this->list_.begin(); iter != this->list_.end(); iter++) {
		free(*iter);
	}
}

UniqueFilterDiffHandler::UniqueFilterDiffHandler(diff_callback * callback,
		void * closure) :
	C_DiffHandler(callback, closure) {
}
}
