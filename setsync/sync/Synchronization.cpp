/*
 * Synchronization.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Synchronization.h"
#include <stdlib.h>

namespace setsync {

namespace sync {

AbstractSyncProcessPart::AbstractSyncProcessPart() :
	optimalPacketSize_(4096) {
}

AbstractSyncProcessPart::~AbstractSyncProcessPart() {
}

void AbstractSyncProcessPart::setOptimalPacketSize(const std::size_t s) {
	this->optimalPacketSize_ = s;
}

std::size_t AbstractSyncProcessPart::getOptimalPacketSize() const {
	return this->optimalPacketSize_;
}
}
}
