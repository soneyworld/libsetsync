/*
 * Progress.cpp
 *
 *  Created on: 29.12.2012
 *      Author: till
 */

#include "Progress.h"

namespace setsync {

namespace sync {

Progress::Progress(int min, int max, bool deterministric) :
	min_(min), max_(max), deterministicProgress_(deterministric) {
	if (min > max) {
		max_ = min;
		min_ = max;
	} else if (min == max) {
		max_++;
	}
	progressPosition_ = min_;
}

Progress::~Progress() {
}

bool Progress::isFinished() const {
	return this->progressPosition_ >= max_ ? true : false;
}
int Progress::getValue() const {
	return this->progressPosition_;
}
void Progress::setValue(const int value) {
	if (value > max_) {
		this->progressPosition_ = max_;
	} else if (value < min_) {
		this->progressPosition_ = min_;
	} else {
		this->progressPosition_ = value;
	}
}
void Progress::setMaximum(const int max) {
	if (max <= this->min_) {
		throw "Illegal Argument";
	}
	if (this->progressPosition_ > max) {
		this->progressPosition_ = max;
	}
	this->max_ = max;
}
int Progress::getMaximum() const {
	return this->max_;
}
void Progress::setMinimum(const int min) {
	if (min >= max_) {
		throw "Illegal Argument";
	}
	if (this->progressPosition_ < min)
		this->progressPosition_ = min;
	this->min_ = min;
}
int Progress::getMinimum() const {
	return this->min_;
}
std::string Progress::getDescription() const {
	return this->progressDescription_;
}
void Progress::setDescription(const std::string desc) {
	this->progressDescription_ = desc;
}

bool Progress::isDeterministic() const {
	return this->deterministicProgress_;
}
void Progress::setDeterministic(const bool deterministic) {
	this->deterministicProgress_ = deterministic;
}

double Progress::getPercentComplete() const {
	double range = max_ - min_;
	return (progressPosition_ - min_) / range;
}
}

}
