/*
 * Synchronization.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SYNCHRONIZATION_H_
#define SYNCHRONIZATION_H_
#include <setsync/DiffHandler.h>

namespace setsync {
class Set;

namespace sync {

class AbstractSyncProcessPart {
public:
	/**
	 * \return true, if more output is available
	 */
	virtual bool pendingOutput() const = 0;
	/**
	 * \return true, if the sync process expects  more input
	 */
	virtual bool awaitingInput() const = 0;

	virtual std::size_t processInput(void * inbuf, const std::size_t length,
			AbstractDiffHandler& diffhandler) = 0;
	virtual std::size_t
			writeOutput(void * outbuf, const std::size_t maxlength) = 0;
};

class SyncableDataStructureInterface {
public:
	virtual AbstractSyncProcessPart * createSyncProcess() = 0;
};

}
}

#endif /* SYNCHRONIZATION_H_ */
