/*
 * SetSync.cpp
 *
 *      Author: Till Lorentzen
 */

#include "SetSync.h"

namespace evaluation {

SetSync::SetSync(const SET_CONFIG config, const size_t initA,
		const size_t initB, const size_t maxElements,
		const size_t sameElements, const SyncType type, const size_t salt) :
	config_(config), initA_(initA), initB_(initB), initMax_(maxElements),
			initSameElements_(sameElements), type_(type), initSalt_(salt) {

}

SetSync::~SetSync() {
}
void SetSync::run() {

}

void SetSync::print(){

}
}
