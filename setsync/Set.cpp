/*
 * Set.cpp
 *
 *  Created on: 12.07.2012
 *      Author: Till Lorentzen
 */
#include "set.h"
#include "Set.hpp"
#include <iostream>

namespace setsync {



Set::Set() {
	std::cout << "Set contructed" << std::endl;

}

Set::~Set() {
	std::cout << "Set deleted" << std::endl;
}

}


int set_init(SET *set){
	setsync::Set * cppset = new setsync::Set();
	set->set = (void *) cppset;
}
int set_free(SET *set){
	setsync::Set * cppset = static_cast<setsync::Set*>( set->set);
	delete cppset;
}
