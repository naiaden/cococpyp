/*
 * ContextValues.h
 *
 *  Created on: Mar 11, 2016
 *      Author: louis
 */

#ifndef HPYPLM_CONTEXTVALUES_H_
#define HPYPLM_CONTEXTVALUES_H_


#include "CoCoInitialiser.h"
#include "utils.h"

class ContextCounts
{


public:
	std::unordered_map<Pattern, long int> contextCounts;

	ContextCounts(SNCBWCoCoInitialiser& cci)
	{
		initialise(cci);
	}

	long int get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern,long int>::const_iterator iter = contextCounts.find(pattern);

		  if ( iter == contextCounts.end() )
		    return 0;
		  else
		    return iter->second;
	}

	void initialise(SNCBWCoCoInitialiser& cci)
	{
		int wordsPerContext = 0 ;
		Pattern previousPrefix = Pattern();
		for(int n = 1; n <= 4; ++n)
		{
			PatternSet<uint64_t> allPatterns = cci.trainPatternModel.extractset(n,n);
			std::cout << "Done extracting set for " << n << std::endl;

			std::set<Pattern, PatternComp> ordered_patterns;

			for(auto pattern : allPatterns)
			{
				ordered_patterns.insert(pattern);
				//ordered_patterns.insert(pattern.reverse());
			}
			std::cout << "Done ordering the set" << std::endl;

			std::vector<int> added_patterns = std::vector<int>();
			for(auto pattern: ordered_patterns)
			{

				//std::cout << pattern.tostring(cci.classDecoder) << "\twpc:" << wordsPerContext << std::endl;

				//Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);
				Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern, 0, n-1);
				//Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern, 1, n);
				//Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern.reverse(), 0, n-1);
				if(prefix != previousPrefix)
				{
					contextCounts[previousPrefix] = wordsPerContext;


//					std::cout << "\t" << previousPrefix.tostring(cci.classDecoder) << "\t" << wordsPerContext << std::endl;
					wordsPerContext = 0;
	//                _general_output << previous_prefix.tostring(cci.getClassDecoder()) << "\t" << added_patterns.size() << "\t" << -llh << "\t" << llh/added_patterns.size() << std::endl;
					previousPrefix = prefix;
				}

				++wordsPerContext;
			}
		}
	}

};


#endif /* HPYPLM_CONTEXTVALUES_H_ */
