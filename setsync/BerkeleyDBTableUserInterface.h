/*
 * BerkeleyDBTableUserInterface.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BERKELEYDBTABLEUSERINTERFACE_H_
#define BERKELEYDBTABLEUSERINTERFACE_H_
#include "db_cxx.h"
namespace berkeley {
class BerkeleyDBTableUserInferface {
public:
	static const char * getLogicalDatabaseName();
	static const DBTYPE getTableType();
	static const u_int32_t getTableFlags() {
		return 0;
	}
};
}
#endif /* BERKELEYDBTABLEUSERINTERFACE_H_ */
