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
	std::unordered_map<Pattern, long int> contextCounts;

public:
	ContextCounts(SNCBWCoCoInitialiser& cci)
	{
		//    for(IndexedCorpus::iterator iter = cci.indexedCorpus->begin(); iter != cci.indexedCorpus->end(); ++iter)
		//	{
		//		for(PatternPointer patternp : cci.trainPatternModel.getreverseindex(iter.index(), 0, 0, 0/*std::stoi(_kORDER)*/))
		//		{
		//			Pattern pattern(patternp);
		//
		//			tsp.printTimeStats();
		//
		//			Pattern context(pattern, 0, std::stoi(_kORDER) - 1);
		//			Pattern focus(pattern, std::stoi(_kORDER)-1, 1);
		//		}
		//	}

		initialise(cci);
	}

	void initialise(SNCBWCoCoInitialiser& cci)
	{
		for(int n = 1; n <= 4; ++n)
		{
			PatternSet<uint64_t> allPatterns = cci.trainPatternModel.extractset(n,n);
			std::cout << "Done extracting set for " << n << std::endl;

			std::set<Pattern, PatternComp> ordered_patterns;

			for(auto pattern : allPatterns)
			{
				ordered_patterns.insert(pattern);
			}
			std::cout << "Done ordering the set" << std::endl;

			Pattern previousPrefix = Pattern();
			int wordsPerContext = 0 ;
			std::vector<int> added_patterns = std::vector<int>();
			for(auto pattern: ordered_patterns)
			{
				Pattern prefix = Pattern(pattern, 0, n-1);
				if(prefix != previousPrefix)
				{
					contextCounts[previousPrefix] = wordsPerContext;
					wordsPerContext = 0;

					std::cout << previousPrefix.tostring(cci.classDecoder) << "\t" << wordsPerContext << std::endl;
	//                _general_output << previous_prefix.tostring(cci.getClassDecoder()) << "\t" << added_patterns.size() << "\t" << -llh << "\t" << llh/added_patterns.size() << std::endl;
					previousPrefix = prefix;
				}

				++wordsPerContext;
			}
		}
	}

};


#endif /* HPYPLM_CONTEXTVALUES_H_ */
