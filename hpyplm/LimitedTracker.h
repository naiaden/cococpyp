/*
 * LimitedTracker.h
 *
 *  Created on: Dec 7, 2016
 *      Author: onrust
 */

#ifndef HPYPLM_LIMITEDTRACKER_H_
#define HPYPLM_LIMITEDTRACKER_H_

#include <map>
#include <vector>

enum class PatternPattern { xxxx=0, xxxd=1, xxcd=3, xbxd=5, axxd=9, xbcd=7, axcd=10, abxd=13, abcd=15 };
enum class LimitedTrackerHitType { prob, weight };

class LimitedTracker {

public:
	void print();

	static LimitedTracker& getInstance()
	{
		static LimitedTracker instance;
		return instance;
	}

	LimitedTracker(LimitedTracker const&) = delete;
	void operator=(LimitedTracker const&) = delete;

	long int registerHit(PatternPattern pp);
	double registerWeight(PatternPattern pp, double weight);

	void reset();
private:
	LimitedTracker() {};

	std::map<PatternPattern, long int> hits;
	std::map<PatternPattern, std::vector<double>> weights;
};

//class EntropyTracker {

#endif /* HPYPLM_LIMITEDTRACKER_H_ */
