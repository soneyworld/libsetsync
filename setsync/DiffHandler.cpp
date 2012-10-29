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

void AbstractDiffHandler::operator()(const unsigned char * hash, const std::size_t hashsize){
	this->handle(hash,hashsize);
}
void ListDiffHandler::handle(const unsigned char * hash,
		const std::size_t hashsize) {
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
}

ListDiffHandler::~ListDiffHandler() {
	std::vector<unsigned char *>::iterator iter;
	for (iter = this->list_.begin(); iter != this->list_.end(); iter++) {
		free(*iter);
	}
}

const unsigned char * ListDiffHandler::operator[](unsigned int const& index) const {
	if (index >= this->list_.size()) {
		return NULL;
	}
	return this->list_[index];
}

const std::size_t ListDiffHandler::size() {
	return this->list_.size();
}

void OutputStreamDiffHandler::handle(const unsigned char * hash,
		const std::size_t hashsize) {
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
		const std::size_t hashsize) {
	(this->callback_)(this->closure_, hash, hashsize);
}
}
