/*
 * Synchronization.h
 *
 *      Author: Till Lorentzen
 */

#ifndef SYNCHRONIZATION_H_
#define SYNCHRONIZATION_H_
#include <setsync/DiffHandler.h>
#include <setsync/utils/CryptoHash.h>

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

	virtual std::size_t getRemainigOutputPacketSize() const = 0;
	/**
	 * \return true, if sync is done
	 */
	virtual bool done() const = 0;
	/**
	 * \return true, if this is fully equal to external entity
	 */
	virtual bool isEqual() const = 0;
};

class SyncableDataStructureInterface {
public:
	virtual AbstractSyncProcessPart * createSyncProcess() = 0;
};

class HashSyncProcessPart: public AbstractSyncProcessPart {
	unsigned char * inHash_;
	unsigned char * outHash_;
	const std::size_t hashsize_;
	std::size_t inPos_;
	std::size_t outPos_;
public:
	HashSyncProcessPart(const utils::CryptoHash& hashFunction,
			const unsigned char * localHashDigit);
	virtual ~HashSyncProcessPart();
	virtual bool pendingOutput() const;
	virtual bool awaitingInput() const;
	virtual std::size_t processInput(void * input, const std::size_t length,
			AbstractDiffHandler& diffhandler);
	virtual std::size_t writeOutput(void * outbuf, const std::size_t maxlength);
	virtual bool isEqual() const;
	virtual bool done() const;
	virtual std::size_t getRemainigOutputPacketSize() const;
};
}
}

#endif /* SYNCHRONIZATION_H_ */
