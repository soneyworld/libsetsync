/*
 * BerkeleyDBTableUserInterface.h
 *
 *      Author: Till Lorentzen
 */

#ifndef BERKELEYDBTABLEUSERINTERFACE_H_
#define BERKELEYDBTABLEUSERINTERFACE_H_
#include "db_cxx.h"
namespace setsync {
namespace storage {
/**
 * Berkeley DB helper classes and interfaces
 */
namespace berkeley {
/**
 * This abstract class must be extended by all classes,
 * which are using a berkeley DB database to save and load
 * data from them. It provides informations about transaction
 * support and holds the parent transaction, if it is available.
 */
class AbstractBdbTableUser {
private:
	/// parent transaction
	DbTxn * txn_;
	/// true, if transactions are available
	const bool txn_enabled_;
public:
	AbstractBdbTableUser(Db * db, DbTxn * txn = NULL);
	virtual ~AbstractBdbTableUser() {
	}
	;
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
	static DBTYPE getTableType();
	/**
	 * A berkeley DB table could require additional flags before
	 * the open method could be called. Normally this is 0. But
	 * if the implementation needs other flags, it should override
	 * this method and should return the correct table flags.
	 *
	 * \return the required flags
	 */
	static u_int32_t getTableFlags() {
		return 0;
	}
	/**
	 * Overrides the existing parent transaction, if txn is NULL,
	 * transaction based methods won't use nested transactions, but
	 * if transactions are enabled, and the parent transaction is NULL,
	 * the methods should use transactions inside.
	 *
	 * \param txn pointer to the parent transaction
	 */
	virtual void setParentTransaction(DbTxn * txn);
	/**
	 * Returns the actual parent transaction pointer. If it has been
	 * set to NULL, also NULL will be returned. So calling this inside
	 * a txn_begin call, will always produce the correct behavior.
	 *
	 * \return a pointer to the actual parent transaction or NULL
	 */
	virtual DbTxn * getParentTransaction(void) const;

	/**
	 * Returns the availability of transactions. This possibility will
	 * be checked in the constructor cannot be changed in the lifetime
	 * of the object.
	 *
	 * \return true, if transactions are available, otherwise false
	 */
	virtual bool isTransactionEnabled(void) const;
};
}
}
}
#endif /* BERKELEYDBTABLEUSERINTERFACE_H_ */
