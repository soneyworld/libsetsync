/*
 * MemStorage.h
 *
 *      Author: Till Lorentzen
 */

#ifndef MEMSTORAGE_H_
#define MEMSTORAGE_H_

#include "KeyValueStorage.h"
#include <map>
#include <string>
#include <stdlib.h>

namespace setsync {

namespace storage {

class MemEntry {
	friend bool operator<(MemEntry const& smaller, MemEntry const& larger);
private:
	unsigned char* data_;
	size_t size_;
public:
	inline int compare(const MemEntry& b) const {
		const int min_len = (size_ < b.size_) ? size_ : b.size_;
		int r = memcmp(data_, b.data_, min_len);
		if (r == 0) {
			if (size_ < b.size_)
				r = -1;
			else if (size_ > b.size_)
				r = +1;
		}
		return r;
	}
	// Return the length (in bytes) of the given data
	size_t size() const {
		return size_;
	}
	/**
	 * Copy constructor
	 */
	MemEntry(const MemEntry& copy) {
		this->size_ = copy.size_;
		this->data_ = (unsigned char *) malloc(size_);
		memcpy(this->data_, copy.data_, size_);
	}

	// Return true iff the length of the given data is zero
	bool empty() const {
		return size_ == 0;
	}
	MemEntry() :
		size_(0) {
		this->data_ = NULL;
	}

	// Create a entry that refers to d[0,n-1].
	MemEntry(const unsigned char* d, size_t n) :
		size_(n) {
		data_ = (unsigned char*) malloc(n);
		memcpy(data_, d, size_);
	}

	// Create a entry that refers to the contents of "s"
	MemEntry(const std::string& s) :
		size_(s.size()) {
		this->data_ = (unsigned char *) malloc(size_);
		memcpy(this->data_, s.data(), size_);
	}

	// Create a entry that refers to s[0,strlen(s)-1]
	MemEntry(const char* s) :
		size_(strlen(s)) {
		this->data_ = (unsigned char *) malloc(size_);
		memcpy(this->data_, s, size_);
	}

	virtual ~MemEntry() {
		if (this->data_ != NULL) {
			free(this->data_);
		}
	}

	// Return a pointer to the beginning of the referenced data
	const unsigned char* data() const {
		return data_;
	}
};

bool operator<(MemEntry const& smaller, MemEntry const& larger) {
	int min = std::min(smaller.size_, larger.size_);
	for (int i = 0; i < min; i++) {
		unsigned char s = smaller.data_[i];
		unsigned char l = larger.data_[i];
		if (s < l) {
			return true;
		} else if (l < s) {
			return false;
		}
	}
	if (smaller.size_ < larger.size_) {
		return true;
	}
	return false;
}

inline bool operator==(const MemEntry& x, const MemEntry& y) {
	return ((x.size() == y.size()) && (memcmp(x.data(), y.data(), x.size())
			== 0));
}

inline bool operator!=(const MemEntry& x, const MemEntry& y) {
	return !(x == y);
}

class MemIterator: public AbstractKeyValueIterator {
private:
	bool valid_;
	const std::map<MemEntry, MemEntry>& map_;
	std::map<MemEntry, MemEntry>::iterator iter_;
public:
	MemIterator(const std::map<MemEntry, MemEntry>& map);
	virtual ~MemIterator();
	virtual void seekToFirst() {
		valid_ = true;
		iter_ = map_.begin();
	}
	virtual bool valid() const {
		return valid_;
	}

	virtual void next() {
		if (iter_ != map_.end())
			iter_++;
		else
			valid_ = false;
	}

	virtual size_t keySize() const {
		return iter_->first.size();
	}

	virtual void key(unsigned char * buffer) const {
		memcpy(buffer, iter_->first.data(), iter_->first.size());
	}
};

class MemStorage: public setsync::storage::AbstractKeyValueStorage {
private:
	std::map<MemEntry, MemEntry> map;
public:
	MemStorage();
	virtual ~MemStorage();
	virtual bool get(const unsigned char * key, const std::size_t length,
			unsigned char ** value, std::size_t * valueSize) const;
	virtual void put(const unsigned char * key, const std::size_t keySize,
			const unsigned char * value, const std::size_t valueSize);
	virtual void del(const unsigned char * key, const std::size_t keySize);
	/**
	 * Cleans map and frees all memory
	 */
	virtual void clear(void);
	virtual AbstractKeyValueIterator * createIterator();
};

}

}

#endif /* MEMSTORAGE_H_ */
