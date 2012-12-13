/*
 * BerkeleyDBTableUserInterface.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BdbTableUser.h"
#include <setsync/utils/BerkeleyDB.h>
namespace berkeley {

AbstractBdbTableUser::AbstractBdbTableUser(Db * db, DbTxn * txn) :
	txn_(txn), txn_enabled_(utils::BerkeleyDB::isTransactionEnabled(db)) {
}

void AbstractBdbTableUser::setParentTransaction(DbTxn * txn) {
	this->txn_ = txn;
}

DbTxn * AbstractBdbTableUser::getParentTransaction(void) const {
	return this->txn_;
}

bool AbstractBdbTableUser::isTransactionEnabled(void) const {
	return this->txn_enabled_;
}

}
