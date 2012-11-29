/*
 * BerkeleyDBTableUserInterface.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BerkeleyDBTableUserInterface.h"
#include <setsync/utils/BerkeleyDB.h>
namespace berkeley {

BerkeleyDBTableUserInferface::BerkeleyDBTableUserInferface(Db * db, DbTxn * txn) :
	txn_(txn), txn_enabled_(utils::BerkeleyDB::isTransactionEnabled(db)) {
}

void BerkeleyDBTableUserInferface::setParentTransaction(DbTxn * txn) {
	this->txn_ = txn;
}

DbTxn * BerkeleyDBTableUserInferface::getParentTransaction(void) {
	return this->txn_;
}

bool BerkeleyDBTableUserInferface::isTransactionEnabled(void) {
	return this->txn_enabled_;
}

}
