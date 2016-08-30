/*
 * LimitedCounts.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */

#include "LimitedCounts.h"





std::string LimitedCounts::name() const
{
	return "limitedcounts";
}

LimitedCounts::LimitedCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts, BackoffStrategy* _backoffStrategy)
{
	backoffStrategy = _backoffStrategy;
	initialise(cci, patternCounts);
}

void LimitedCounts::initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
{
	PatternSet<uint64_t> allFocusWords = cci.trainPatternModel.extractset(1,1);
	long int numberOfFocusWords = allFocusWords.size();

	for(int n = 1; n <= kORDER; ++n)
	{
		std::cout << "Extracting LimitedCounts set for " << n << "...";
		PatternSet<uint64_t> allPatterns;
		if(n == 1)
		{
			allPatterns = allFocusWords;
		} else
		{
			allPatterns = cci.trainPatternModel.extractset(n,n);
		}
		std::cout << " done" << std::endl;

		PatternSet<uint64_t> allContexts;
		for(auto pattern : allPatterns)
		{
			Pattern context = (n==1) ? Pattern() : Pattern(pattern, 0, n-1);
			allContexts.insert(context);
		}

		for(auto context : allContexts)
		{
			LimitedInformation li;
			long int nobackoff = 0;
			double q0 = 0.0;
			for(auto focus : allFocusWords)
			{

				Pattern pattern = context + focus;
				if(patternCounts->get(pattern))
				{ // not oov
					++nobackoff;
					std::cout << "NOT OOV" << pattern.tostring(cci.classDecoder) << std::endl;
					q0 += backoffStrategy->prob(focus, context, "" /*focus.tostring(cci.classDecoder)*/);


				}
			}
			std::cout << "Q0 ===== " << q0 << std::endl;
			li.nobackoff = nobackoff;
			li.backoff = numberOfFocusWords - nobackoff;
			li.q0 = q0;
		}
	}
}

LimitedInformation LimitedCounts::get(const Pattern& pattern, CoCoInitialiser * const cci) const
{
	std::unordered_map<Pattern, LimitedInformation>::const_iterator iter = limitedCounts.find(pattern);
	if ( iter != limitedCounts.end() )
	{
		return iter->second;
	} else
	{
		return LimitedInformation();//iter->second;
	}
}

