/*
 * Synchronization.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SYNCHRONIZATION_H_
#define SYNCHRONIZATION_H_

namespace setsync {

class AbstractSynchronizationProcess {
public:
	AbstractSynchronizationProcess();
	virtual ~AbstractSynchronizationProcess();
	virtual void bloomfilterSync() = 0;
	virtual void trieSync() = 0;
};

class NetworkSynchronizationProcess: public AbstractSynchronizationProcess {

};

class IPCSynchronizationProcess: public AbstractSynchronizationProcess {

};

class DirectSynchronizationProcess: public AbstractSynchronizationProcess {

};
}

#endif /* SYNCHRONIZATION_H_ */
