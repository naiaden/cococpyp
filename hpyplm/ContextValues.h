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

class ContextValues
{
public:
	virtual double get(const Pattern& pattern) const = 0;
};

class UniformCounts : public ContextValues
{
public:
	UniformCounts(SNCBWCoCoInitialiser& cci)
	{
	}

	double get(const Pattern& pattern) const
	{
		return 1.0;
	}
};

class MLECounts : public ContextValues
{
	public:
	std::unordered_map<Pattern, double> mleCounts;

	long int V = 0;

	MLECounts(SNCBWCoCoInitialiser& cci)
	{
		initialise(cci);
	}

	double get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern, double>::const_iterator iter = mleCounts.find(pattern);

		  if ( iter == mleCounts.end() )
		    return 0;
		  else
		    return iter->second;
	}

	void initialise(SNCBWCoCoInitialiser& cci)
	{
		Pattern previousPrefix = Pattern();
		double llh = 0;
		int sum = 0;

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

			std::vector<int> added_patterns = std::vector<int>();
			for(auto pattern: ordered_patterns)
			{
				Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern, 0, n-1);
				if(prefix != previousPrefix)
				{
					for(auto count : added_patterns)
					{
						double mle = count*1.0/sum;
						llh -= log(mle);
					}

					mleCounts[previousPrefix] = llh;

					llh = 0;
					sum = 0;
					added_patterns = std::vector<int>();
					previousPrefix = prefix;
				}

				int count = cci.trainPatternModel.occurrencecount(pattern);
				sum += count;
				added_patterns.push_back(count);
			}
		}

		V = get(Pattern());
	}

};

class ContextCounts
{


public:
	std::unordered_map<Pattern, long int> contextCounts;

	long int V = 0;

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
			}
			std::cout << "Done ordering the set" << std::endl;

			std::vector<int> added_patterns = std::vector<int>();
			for(auto pattern: ordered_patterns)
			{
				Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern, 0, n-1);
				if(prefix != previousPrefix)
				{
					contextCounts[previousPrefix] = wordsPerContext;

					wordsPerContext = 0;
					previousPrefix = prefix;
				}

				++wordsPerContext;
			}
		}

		V = get(Pattern());
	}

};


#endif /* HPYPLM_CONTEXTVALUES_H_ */
