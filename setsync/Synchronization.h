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
/**
 * With the help of this class, an external set can be synchronized with
 * the local instance.
 */
class SynchronizationProcess {
private:
	enum status {
		START, BF, TRIE, EQUAL
	} stat_;
	/// Local set instance
	Set * set_;
	/// Position of bloom filter process
	std::size_t pos_;
	/// default handler, C++ interface only, NULL if the no diff handler should be used
	AbstractDiffHandler * handler_;
	/// hash of the other trie root
	unsigned char * externalhash;
public:
	/**
	 * Creates a new Synchronization Process for the given set. If a
	 * diff handler is passed, it will be used as default for the step
	 * call, which hasn't passed a dedicated callback, or DiffHandler
	 * reference.
	 *
	 * \param set local set, which should be synchronized
	 * \param handler which should be called as default, if no other is given
	 */
	SynchronizationProcess(Set * set, AbstractDiffHandler * handler = NULL);
	std::size_t step(void * inbuf, const std::size_t inlength, void * outbuf,
			const std::size_t outlength, diff_callback * callback,
			void * closure);
	std::size_t step(void * inbuf, const std::size_t inlength, void * outbuf,
			const std::size_t outlength, AbstractDiffHandler& diffhandler);
	std::size_t step(void * inbuf, const std::size_t inlength, void * outbuf,
			const std::size_t outlength);
	virtual ~SynchronizationProcess();
	/**
	 * Calculates the optimal size of a sending buffer for the given network parameter.
	 *
	 * \param RTT round trip time in nanoseconds
	 * \param bandwidth in bits per second
	 * \return the optimal size of a sending buffer
	 */
	std::size_t
	calcOutputBufferSize(const size_t RTT, const size_t bandwidth) const;
	/**
	 * \return true, if sync is done
	 */
	virtual bool done() const;
};
}

#endif /* SYNCHRONIZATION_H_ */
