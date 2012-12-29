/*
 * Progress.h
 *
 *      Author: Till Lorentzen
 */

#ifndef PROGRESS_H_
#define PROGRESS_H_
#include <string>

namespace setsync {

namespace sync {

class Progress {
private:
	std::string progressDescription_;
	int min_;
	int max_;
	int progressPosition_;
	bool deterministicProgress_;
public:
	Progress(int min = 0, int max = 100, bool deterministic = true);
	virtual ~Progress();
	virtual bool isFinished() const;
	virtual int getValue() const;
	virtual void setValue(const int value);
	virtual void setMaximum(const int max);
	virtual int getMaximum() const;
	virtual void setMinimum(const int min);
	virtual int getMinimum() const;
	virtual std::string getDescription() const;
	virtual void setDescription(const std::string desc);
	virtual bool isDeterministic() const;
	virtual void setDeterministic(const bool deterministic);
	/**
	 * Returns the percent complete for the progress.
	 * Note that this number is between 0.0 and 1.0.
	 *
	 * \return the percent complete for this progress
	 */
	virtual double getPercentComplete() const;
};

}

}

#endif /* PROGRESS_H_ */
