/*
 * DbIndex.cpp
 *
 *      Author: Till Lorentzen
 */

#include "BdbIndex.h"
#include <setsync/utils/BerkeleyDB.h>

namespace setsync {

namespace index {

BdbIndex::BdbIndex(const utils::CryptoHash& hash, Db * db) :
	AbstractSetIndex(hash), berkeley::AbstractBdbTableUser(db), db_(db) {

}

BdbIndex::~BdbIndex() {
}

bool BdbIndex::put(const unsigned char * hash, const void * data,
		const std::size_t length) {
	//TODO implement
	throw "not yet implemented";
	return false;
}

bool BdbIndex::remove(const unsigned char * hash) {
	//TODO implement
	throw "not yet implemented";
	return false;
}

void BdbIndex::clear(void) {
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

size_t BdbIndex::getSizeOf(const unsigned char * hash) const {
	//TODO implement
	throw "not yet implemented";
	return 0;
}

bool BdbIndex::get(const unsigned char * hash, void * buffer,
		size_t * buffersize) const {

}
} // namespace index

} // namespace setsync
