/*
 * AbstractSynchronizationStrategy.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SyncStrategy.h"

namespace setsync {

SynchronizationStrategy::SynchronizationStrategy(
		AbstractBloomFilterSyncStrategy& bfstrategy,
		AbstractTrieSyncStrategy& triestrategy) :
	bfstrategy_(bfstrategy), triestragegy_(triestrategy) {
}

SynchronizationStrategy::~SynchronizationStrategy() {
}

AbstractBloomFilterSyncStrategy& SynchronizationStrategy::getBloomFilterSyncStrategy() {
	return this->bfstrategy_;
}
AbstractTrieSyncStrategy& SynchronizationStrategy::getTrieSyncStrategy() {
	return this->triestragegy_;
}
}
