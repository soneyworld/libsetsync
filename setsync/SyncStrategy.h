/*
 * AbstractSynchronizationStrategy.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SYNCSTRATEGY_H_
#define SYNCSTRATEGY_H_

namespace setsync {

class AbstractBloomFilterSyncStrategy {

};

class AbstractTrieSyncStrategy {

};

class SynchronizationStrategy {
protected:
	AbstractBloomFilterSyncStrategy& bfstrategy_;
	AbstractTrieSyncStrategy& triestragegy_;
public:
	SynchronizationStrategy(
			AbstractBloomFilterSyncStrategy& bfstrategy,
			AbstractTrieSyncStrategy& triestrategy);
	virtual ~SynchronizationStrategy();
	virtual AbstractBloomFilterSyncStrategy& getBloomFilterSyncStrategy();
	virtual AbstractTrieSyncStrategy& getTrieSyncStrategy();
};

}

#endif /* SYNCSTRATEGY_H_ */
