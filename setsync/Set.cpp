/*
 * Set.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Set.hpp"
#include "MemSet.h"
#include <iostream>

namespace setsync {

Set::Set() {
	this->size = 0;
	//TODO must be configurable
	this->maxSize = 1000;

}

Set::~Set() {

}

bool Set::isEmpty() const {
	return (this->size == 0) ? true : false;
}

size_t Set::getSize() const {
	return this->size;
}
size_t Set::getMaximumSize() const {
	return this->maxSize;
}

void Set::setSize(const size_t s) {
	this->size = s;
}
void Set::setMaximumSize(const size_t s){
	this->maxSize = s;
}

}



int set_init(SET *set) {
	setsync::Set * cppset = new setsync::MemSet();
	set->set = (void *) cppset;
}

int set_free(SET *set) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	delete cppset;
}

// Lookup
int find(SET *set, const char * key) {
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->find(key);
}

// Modifiers
int set_insert(SET *set, const char * key){
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->insert(key);
}

int set_erase(SET *set, const char * key){
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	return cppset->erase(key);
}

int set_clear(SET *set){
	setsync::Set * cppset = static_cast<setsync::Set*> (set->set);
	cppset->clear();
	return 0;

}
