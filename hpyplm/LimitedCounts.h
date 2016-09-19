/*
 * LimitedCounts.h
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */

#ifndef HPYPLM_LIMITEDCOUNTS_H_
#define HPYPLM_LIMITEDCOUNTS_H_

//#include <classencoder.h>
//#include <classdecoder.h>
//#include <patternmodel.h>

//#include <map>

#include "CoCoInitialiser.h"

class BackoffStrategy;
class PatternCounts;

struct LimitedInformation
{
	long int backoff = 0;
	long int nobackoff = 0;
	double P = 0.0; // sum over all p0
};

//class LimitedCounts
//{
//public:
//	std::string name() const;
//
//	std::unordered_map<Pattern, LimitedInformation> limitedCounts;
//	BackoffStrategy * backoffStrategy;
//	std::ofstream probsFile;
//	PatternSet<uint64_t> allFocusWords;
//
//	LimitedCounts(SNCBWCoCoInitialiser& cci, const std::string& fileName);
//	LimitedCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts, BackoffStrategy* _backoffStrategy);
//	~LimitedCounts();
//
//	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);
//
//	LimitedInformation get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr);
//};


class LimitedCountsCache
{
public:
	std::string name() const;

	std::unordered_map<Pattern, LimitedInformation> limitedCounts;
	PatternCounts * patternCounts;
	BackoffStrategy * backoffStrategy;
	std::ofstream probsFile;

	PatternSet<uint64_t> allFocusWords;
	long int numberOfFocusWords = 0;

	LimitedCountsCache(SNCBWCoCoInitialiser& cci, const std::string& fileName);
	LimitedCountsCache(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts, BackoffStrategy* _backoffStrategy);
	~LimitedCountsCache();

	void initialise(SNCBWCoCoInitialiser& cci);


	LimitedInformation get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr);
};


#endif /* HPYPLM_LIMITEDCOUNTS_H_ */
