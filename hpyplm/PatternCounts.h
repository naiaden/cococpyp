/*
 * PatternCounts.h
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */

#ifndef HPYPLM_PATTERNCOUNTS_H_
#define HPYPLM_PATTERNCOUNTS_H_



#include <unordered_map>
#include <pattern.h>
#include "CoCoInitialiser.h"

/**
 * For each context (or pattern) we store how often it occurs
 * (according to the train data)
 */
class PatternCounts
{

public:
	std::unordered_map<Pattern, long int> patternCounts;



	void fromFile(SNCBWCoCoInitialiser& cci);

	long int get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) const;
};


#endif /* HPYPLM_PATTERNCOUNTS_H_ */
