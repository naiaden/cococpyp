/*
 * ContextCounts.h
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */

#ifndef HPYPLM_CONTEXTCOUNTS_H_
#define HPYPLM_CONTEXTCOUNTS_H_

#include <unordered_map>
#include <pattern.h>

/**
 * For each context we store by how many focus words it can be followed
 * (according to the train data)
 */
class ContextCounts
{

public:

	std::unordered_map<Pattern, long int> contextCounts;
	long int V = 0;
	Pattern oneSkipPattern = Pattern();

	// The empty pattern can be followed by any word, so get(Pattern()) = V
	//
	void fromFile(SNCBWCoCoInitialiser& cci);

	long int get(const Pattern& pattern) const;

	void fromData(SNCBWCoCoInitialiser& cci);

	void count(const std::set<Pattern>& orderedPatterns);

};



#endif /* HPYPLM_CONTEXTCOUNTS_H_ */
