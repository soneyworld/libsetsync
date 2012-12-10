/*
 * Synchronization.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SYNCHRONIZATION_H_
#define SYNCHRONIZATION_H_
#include <setsync/Set.hpp>
#include <setsync/DiffHandler.h>


namespace setsync {
class Set;

class AbstractSynchronizationProcess {
private:
	Set * set_;
public:
	AbstractSynchronizationProcess(Set * set, AbstractDiffHandler& handler);
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