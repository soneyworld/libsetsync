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
#include "StopWatch.h"

using namespace std;

namespace evaluation {

enum SyncType {
	LOOSE, STRICT, BOTH
};

class SetSync: public Test {
private:
	const size_t initA_;
	const size_t initB_;
	const size_t initSameElements_;
	const size_t bufferSize_;
	const SyncType type_;
	const string initSalt_;
	string dbType_;
	string hashType_;
	setsync::config::Configuration configA_;
	setsync::config::Configuration configB_;
	setsync::Set A_;
	setsync::Set B_;
	StopWatch watchA;
	StopWatch watchB;
	StopWatch insertWatchA;
	StopWatch insertWatchB;
	uint64_t diffSize_;
	void runLooseSync(setsync::SynchronizationProcess * processA,
			setsync::SynchronizationProcess * processB);
	void runStrictSync(setsync::SynchronizationProcess * processA,
			setsync::SynchronizationProcess * processB);
	void printLine(setsync::SynchronizationProcess * processA,
			setsync::SynchronizationProcess * processB, const string& phase) const;
public:
	SetSync(const SET_CONFIG config, const size_t initA, const size_t initB,
			const size_t sameElements, const SyncType type, const string salt,
			const size_t maximumBufferSize);
	virtual ~SetSync();
	virtual void run();
};

}

#endif /* SETSYNC_H_ */
