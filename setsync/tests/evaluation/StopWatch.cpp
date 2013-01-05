/*
 * StopWatch.cpp
 *
 *      Author: Till Lorentzen
 */

#include "StopWatch.h"

StopWatch::StopWatch() :
	time_duration_(0), time_last_duration_(0), cpu_start_(0), cpu_stop_(0),
			cpu_duration_(0), cpu_last_duration_(0) {
	time_start_.tv_sec = 0;
	time_start_.tv_nsec = 0;
	time_stop_.tv_sec = 0;
	time_stop_.tv_nsec = 0;
}

StopWatch::~StopWatch() {
}

void StopWatch::start() {
	clock_gettime(CLOCK_REALTIME, &time_start_);
	this->cpu_start_ = clock();
}
void StopWatch::stop() {
	this->cpu_stop_ = clock();
	clock_gettime(CLOCK_REALTIME, &time_stop_);
	this->cpu_last_duration_ = this->cpu_stop_ - this->cpu_start_;
	this->cpu_last_duration_ = this->cpu_last_duration_ / CLOCKS_PER_SEC;
	this->cpu_duration_ += this->cpu_last_duration_;
	long seconds = this->time_stop_.tv_sec - this->time_start_.tv_sec;
	long int nanoseconds = this->time_stop_.tv_nsec - this->time_start_.tv_nsec;
	this->time_last_duration_ = seconds;
	this->time_last_duration_ += (double) (nanoseconds) / 1000000000;

	this->time_duration_ += this->time_last_duration_;
}
void StopWatch::reset() {
	this->stop();
	this->cpu_duration_ = 0;
	this->time_duration_ = 0;
}

double StopWatch::getDuration() const {
	return this->time_duration_;
}

double StopWatch::getLastDuration() const {
	return this->time_last_duration_;
}

double StopWatch::getCPUDuration() const {
	return this->cpu_duration_;
}

double StopWatch::getLastCPUDuration() const {
	return this->cpu_last_duration_;
}
