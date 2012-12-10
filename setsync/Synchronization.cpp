/*
 * Synchronization.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Synchronization.h"

namespace setsync {

SynchronizationProcess::SynchronizationProcess(Set * set,
		AbstractDiffHandler * handler) :
	set_(set), handler_(handler), pos_(0), stat_(START) {
	this->externalhash = new unsigned char[set_->hash_.getHashSize()];
}

std::size_t SynchronizationProcess::step(void * inbuf,
		const std::size_t inlength, void * outbuf, const std::size_t outlength,
		diff_callback * callback, void * closure) {
	C_DiffHandler handler(callback, closure);
	return this->step(inbuf, inlength, outbuf, outlength, handler);
}

std::size_t SynchronizationProcess::step(void * inbuf,
		const std::size_t inlength, void * outbuf, const std::size_t outlength,
		AbstractDiffHandler& diffhandler) {
	std::size_t hashsize = this->set_->hash_.getHashSize();
	switch (this->stat_) {
	case START: {
		std::size_t min = std::min(inlength, hashsize - pos_);
		memcpy(externalhash + pos_, inbuf, min);
		pos_ += min;
		if (pos_ == hashsize) {
			unsigned char localroot[hashsize];
			if (this->set_->trie_->getRoot(localroot)) {
				if (memcmp(localroot, externalhash, hashsize) == 0) {
					this->stat_ = EQUAL;
				} else {
					this->stat_ = BF;
					this->pos_ = 0;
					return step((unsigned char *) (inbuf) + min,
							inlength - min, outbuf, outlength, diffhandler);
				}
			} else {
				this->stat_ = BF;
				this->pos_ = 0;
				return step((unsigned char *) (inbuf) + min, inlength - min,
						outbuf, outlength, diffhandler);
			}
		}
	}
		break;
	case BF: {
		std::size_t bflength = this->set_->bf_->size();
		unsigned char localroot[hashsize];
		if (pos_ + inlength > bflength) {
			std::size_t bfpart = inlength - (bflength - pos_);
			this->set_->bf_->diff((unsigned char *) inbuf, bfpart, pos_,
					diffhandler);
			this->stat_ = TRIE;
			this->pos_ = 0;
			return step((unsigned char *) inbuf + bfpart, inlength - bfpart,
					outbuf, outlength, diffhandler);
		} else if (pos_ + inlength == bflength) {
			this->set_->bf_->diff((unsigned char *) inbuf, inlength, pos_,
					diffhandler);
			this->stat_ = TRIE;
			this->pos_ = 0;
			return 0;
		} else {
			this->set_->bf_->diff((unsigned char *) inbuf, inlength, pos_,
					diffhandler);
			this->pos_ += inlength;
			return 0;
		}
	}
		break;
	case TRIE: {

	}
		break;
	case EQUAL: {

	}
		break;
	}
	//TODO
	throw "not yet implemented";
}

std::size_t SynchronizationProcess::step(void * inbuf,
		const std::size_t inlength, void * outbuf, const std::size_t outlength) {
	if (this->handler_ != NULL) {
		return this->step(inbuf, inlength, outbuf, outlength, *handler_);
	} else {
		IgnoringDiffHandler ignore;
		return this->step(inbuf, inlength, outbuf, outlength, ignore);
	}
}

SynchronizationProcess::~SynchronizationProcess() {
	delete this->externalhash;
}

std::size_t SynchronizationProcess::calcOutputBufferSize(const size_t RTT,
		const size_t bandwidth) const {
	if (RTT == 0) {
		return this->set_->hash_.getHashSize() * 2;
	} else {
		double rtt = RTT;
		rtt = rtt / 1000000;
		double bw = bandwidth;
		double bits = bandwidth * rtt;
		std::size_t result = (std::size_t) (bits / 8.0);
		return result;
	}
}

bool SynchronizationProcess::done() const {
	if (this->stat_ == EQUAL) {
		return true;
	} else {
		return false;
	}
}
}
