/*
 * DbIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "DbIndex.h"
#include <setsync/utils/BerkeleyDB.h>

namespace setsync {

namespace index {

DbIndex::DbIndex(const utils::CryptoHash& hash, Db * db) :
	AbstractSetIndex(hash), berkeley::BerkeleyDBTableUserInferface(db), db_(db) {

}

DbIndex::~DbIndex() {
}

bool DbIndex::put(const unsigned char * hash, const void * data,
		const std::size_t length) {
	//TODO implement
	throw "not yet implemented";
	return false;
}

bool DbIndex::remove(const unsigned char * hash) {
	//TODO implement
	throw "not yet implemented";
	return false;
}

void DbIndex::clear(void) {
	u_int32_t counter;
	if (isTransactionEnabled()) {
		DbTxn * txn;
		this->db_->get_env()->txn_begin(this->getParentTransaction(), &txn, 0);
		try {
			if(this->db_->truncate(txn, &counter, 0)==0)
				txn->commit(0);
			else
				txn->abort();
		} catch (DbException e) {
			txn->abort();
		}
	} else {
		this->db_->truncate(NULL, &counter, 0);
	}
}

size_t DbIndex::getSizeOf(const unsigned char * hash) const {
	//TODO implement
	throw "not yet implemented";
	return 0;
}

bool DbIndex::get(const unsigned char * hash, void * buffer,
		const size_t * buffersize) const {

}
} // namespace index

} // namespace setsync
