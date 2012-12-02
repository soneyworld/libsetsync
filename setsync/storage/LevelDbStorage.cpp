/*
 * LevelDbStorage.cpp
 *
 *      Author: Till Lorentzen
 */

#include "LevelDbStorage.h"
#include <setsync/utils/FileSystem.h>

namespace setsync {

namespace storage {

LevelDbStorage::LevelDbStorage(const std::string& path) :
	path_(path) {
	options_.create_if_missing = true;
	leveldb::Status s = leveldb::DB::Open(options_, path_, &(this->db_));
}

LevelDbStorage::~LevelDbStorage() {
	delete this->db_;
}

bool LevelDbStorage::get(const unsigned char * key, const std::size_t length,
		unsigned char ** value, std::size_t * valueSize) const {
	std::string v;
	leveldb::Slice k((char *) key, length);
	leveldb::Status s = this->db_->Get(readOptions_, k, &v);
	if (s.ok()) {
		*value = (unsigned char *) malloc(v.size());
		memcpy(*value, v.c_str(), v.size());
		*valueSize = v.size();
		return true;
	} else {
		*value = NULL;
		*valueSize = 0;
		return false;
	}
}
void LevelDbStorage::put(const unsigned char * key, const std::size_t keySize,
		const unsigned char * value, const std::size_t valueSize) {
	leveldb::Slice k((char *) key, keySize);
	leveldb::Slice v((char *) value, valueSize);
	leveldb::Status s = this->db_->Put(this->writeOptions_, k, v);
	if (!s.ok()) {
		throw s.ToString();
	}
}
void LevelDbStorage::del(const unsigned char * key, const std::size_t keySize) {
	leveldb::Slice k((char*) key, keySize);
	leveldb::Status s = this->db_->Delete(writeOptions_, k);
	if (!s.ok()) {
		throw s.ToString();
	}
}

void LevelDbStorage::clear(void) {
	delete this->db_;
	utils::FileSystem::rmDirRecursive(this->path_.c_str());
	leveldb::Status s = leveldb::DB::Open(options_, path_, &(this->db_));
}

}

}
