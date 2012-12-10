/*
 * Synchronization.cpp
 *
 *      Author: Till Lorentzen
 */

#include "Synchronization.h"

namespace setsync {

AbstractSynchronizationProcess::AbstractSynchronizationProcess(Set * set, AbstractDiffHandler& handler) : set_(set) {

}

AbstractSynchronizationProcess::~AbstractSynchronizationProcess() {
}

}
