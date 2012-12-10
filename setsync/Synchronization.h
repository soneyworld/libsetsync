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
	/// Local set instance
	Set * set_;
	/// Position state of the actual sync process
	std::size_t pos_;
	/// default handler, C++ interface only, NULL if the no diff handler should be used
	AbstractDiffHandler * handler_;
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
	ssize_t step(void * inbuf, const std::size_t inlength, void * outbuf,
			const std::size_t outlength, diff_callback * callback,
			void * closure);
	ssize_t step(void * inbuf, const std::size_t inlength, void * outbuf,
			const std::size_t outlength, AbstractDiffHandler& diffhandler);
	ssize_t step(void * inbuf, const std::size_t inlength, void * outbuf,
			const std::size_t outlength);
	virtual ~SynchronizationProcess();
	/**
	 * Calculates the optimal size of a sending buffer for the given network parameter.
	 */
	size_t
	calcOutputBufferSize(const size_t RTT, const size_t bandwidth) const;
};
}

#endif /* SYNCHRONIZATION_H_ */
