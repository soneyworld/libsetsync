/*
 * StopWatch.h
 *
 *      Author: Till Lorentzen
 */

#ifndef STOPWATCH_H_
#define STOPWATCH_H_
#include <iostream>
#include <time.h>
/**
 * This class provides methods to stop durations of calculations
 * by saving the CPU time, consumed by the calling process, and the
 * real world time. The CPU time is helpful for algorithms without I/O.
 * If the waiting duration of I/O Operations is also needed, the real
 * world time is provided.
 */
class StopWatch {
private:
	/// Real world time of the last start
	timespec time_start_;
	/// Real world time of the last stop
	timespec time_stop_;
	/// Complete duration in real world, consumed by the intervals
	double time_duration_;
	/// Real world duration of the last start-stop interval
	double time_last_duration_;
	/// CPU time of the last start
	clock_t cpu_start_;
	/// CPU time of the last stop
	clock_t cpu_stop_;
	/// Complete duration of CPU time, consumed by the intervals
	double cpu_duration_;
	/// CPU duration of the last start-stop interval
	double cpu_last_duration_;
public:
	StopWatch();
	/**
	 * Starts the time measurement
	 */
	void start();
	/**
	 * Stops the time measurement and recalculates the durations
	 */
	void stop();
	/**
	 * Resets all durations and all timer
	 */
	void reset();
	/**
	 * Returns the sum of all start stop intervals in seconds
	 *
	 * \return the sum duration in seconds
	 */
	double getDuration();
	/**
	 * \return last interval duration in seconds
	 */
	double getLastDuration();
	/**
	 * \return sum of cpu duration in seconds
	 */
	double getCPUDuration();
	/**
	 * \return last interval duration of CPU time in seconds
	 */
	double getLastCPUDuration();
	/// default destructor
	virtual ~StopWatch();
};

#endif /* STOPWATCH_H_ */
