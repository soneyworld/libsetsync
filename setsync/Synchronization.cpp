/*
 * Synchronization.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Synchronization.h"

namespace setsync {

SynchronizationProcess::SynchronizationProcess(Set * set,
		AbstractDiffHandler * handler) :
	set_(set), handler_(handler), pos_(0) {
}

std::size_t SynchronizationProcess::step(void * inbuf, const std::size_t inlength,
		void * outbuf, const std::size_t outlength, diff_callback * callback,
		void * closure) {
	C_DiffHandler handler(callback, closure);
	return this->step(inbuf, inlength, outbuf, outlength, handler);
}

std::size_t SynchronizationProcess::step(void * inbuf, const std::size_t inlength,
		void * outbuf, const std::size_t outlength,
		AbstractDiffHandler& diffhandler) {
	//TODO
	throw "not yet implemented";
}

std::size_t SynchronizationProcess::step(void * inbuf, const std::size_t inlength,
		void * outbuf, const std::size_t outlength) {
	if (this->handler_ != NULL) {
		return this->step(inbuf, inlength, outbuf, outlength, *handler_);
	} else {
		IgnoringDiffHandler ignore;
		return this->step(inbuf, inlength, outbuf, outlength, ignore);
	}
}

SynchronizationProcess::~SynchronizationProcess() {
}

std::size_t SynchronizationProcess::calcOutputBufferSize(const size_t RTT,
		const size_t bandwidth) const {
	if (RTT == 0) {
		return this->set_->hash_.getHashSize() * 2;
	} else {
		//TODO
		throw "not yet implemented!";
	}
}
}
