/*
 * BerkeleyDBTableUserInterface.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BERKELEYDBTABLEUSERINTERFACE_H_
#define BERKELEYDBTABLEUSERINTERFACE_H_
#include "db_cxx.h"
/**
 * Berkeley DB helper classes and interfaces
 */
namespace berkeley {
/**
 * This Interface must be implemented by all classes,
 * which are using a berkeley DB database to save and load
 * data from them.
 */
class BerkeleyDBTableUserInferface {
private:
	DbTxn * txn_;
	const bool txn_enabled_;
public:
	BerkeleyDBTableUserInferface(Db * db, DbTxn * txn = NULL);
	virtual ~BerkeleyDBTableUserInferface(){};
	/**
	 * Should return the logical database table name, which must
	 * be passed to the berkeley DB open method as parameter.
	 *
	 * \return the logical database name
	 */
	static const char * getLogicalDatabaseName();
	/**
	 * Every Berkeley DB table has got a type. This method should
	 * return the type, required of the class, using the berkeley
	 * DB table.
	 *
	 * \return the table type
	 */
	static const DBTYPE getTableType();
	/**
	 * A berkeley DB table could require additional flags before
	 * the open method could be called. Normally this is 0. But
	 * if the implementation needs other flags, it should override
	 * this method and should return the correct table flags.
	 *
	 * \return the required flags
	 */
	static const u_int32_t getTableFlags() {
		return 0;
	}
	virtual void setParentTransaction(DbTxn * txn);
	virtual DbTxn * getParentTransaction(void);
	virtual bool isTransactionEnabled(void);
};
}
#endif /* BERKELEYDBTABLEUSERINTERFACE_H_ */
