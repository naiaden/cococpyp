/*
 * LimitedCounts.h
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */

#ifndef HPYPLM_LIMITEDCOUNTS_H_
#define HPYPLM_LIMITEDCOUNTS_H_

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <map>


struct LimitedInformation
{
	long int backoff = 0;
	long int nobackoff = 0;
	double q0 = 0.0; // (1-sum over all p0)
};




class LimitedCounts
{
public:
	std::string name() const;

	std::unordered_map<Pattern, LimitedInformation> limitedCounts;
	BackoffStrategy * backoffStrategy;

	LimitedCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts, BackoffStrategy* _backoffStrategy);

	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);

	LimitedInformation get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) const;
};


#endif /* HPYPLM_LIMITEDCOUNTS_H_ */
