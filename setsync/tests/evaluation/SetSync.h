/*
 * SetSync.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SETSYNC_H_
#define SETSYNC_H_

#include "Test.h"
#include <setsync/Set.hpp>
#include <setsync/set.h>

using namespace std;

namespace evaluation {

enum SyncType {
	LOOSE, STRICT, BOTH
};

class SetSync: public Test {
private:
	const SET_CONFIG config_;
	const size_t initA_;
	const size_t initB_;
	const size_t initMax_;
	const size_t initSameElements_;
	const SyncType type_;
	const size_t initSalt_;
public:
	SetSync(const SET_CONFIG config, const size_t initA, const size_t initB,
			const size_t maxElements, const size_t sameElements,
			const SyncType type, const size_t salt);
	virtual ~SetSync();
	virtual void run();
	virtual void print();
};

}

#endif /* SETSYNC_H_ */
