/*
 * DiffHandler.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DiffHandler.h"
#include <stdlib.h>
#include <string.h>

namespace setsync {

ListDiffHandler::ListDiffHandler() {

}
void ListDiffHandler::handle(const unsigned char * hash,
		const std::size_t hashsize) {
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
}
