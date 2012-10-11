/*
 * BerkeleyDBTableUserInterface.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BERKELEYDBTABLEUSERINTERFACE_H_
#define BERKELEYDBTABLEUSERINTERFACE_H_
#include "db_cxx.h"

class BerkeleyDBTableUserInferface {
public:
	static const char * getLogicalDatabaseName() const = 0;
	static const enum DBTYPE getTableType() const = 0;
};

#endif /* BERKELEYDBTABLEUSERINTERFACE_H_ */
