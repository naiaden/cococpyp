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

/**
 * For each context (or pattern) we store how often it occurs
 * (according to the train data)
 */
class PatternCounts
{

public:
	std::unordered_map<Pattern, long int> patternCounts;

	void fromFile(SNCBWCoCoInitialiser& cci, const std::string& fileName)
	{
		std::ifstream file(fileName);
		std::string   line;

		const bool allowUnknown = false;
		const bool autoAddUnknown = false;

		while(std::getline(file, line))
		{
			std::stringstream   linestream(line);
			std::string         patternString;
			std::string         patternCountString;
			long int            patternCount;

			std::getline(linestream, patternString, '\t');
			std::getline(linestream, patternCountString, '\t');
			patternCount = std::stol(patternCountString);

			Pattern pattern = cci.classEncoder.buildpattern(patternString, allowUnknown, autoAddUnknown);

			patternCounts[pattern] = patternCount;
			std::cout << "P:" << pattern.tostring(cci.classDecoder) << " C:" << patternCount << std::endl;
		}
	}

	long int get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern,long int>::const_iterator iter = patternCounts.find(pattern);

		  if ( iter != patternCounts.end() )
			return iter->second;//0;
		  else
			return 0;//iter->second;
	}
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

	MLECounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts = nullptr)
	{
		initialise(cci, patternCounts);
	}

	double get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern, double>::const_iterator iter = mleCounts.find(pattern);

		  if ( iter == mleCounts.end() )
		    return 0;
		  else
		    return iter->second;
	}



	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts = nullptr)
	{
		if(patternCounts)
		{
			std::cout << "I'M USING PATTERNCOUNTS" << std::endl;
		}

		Pattern previousPrefix = Pattern();
		double llh = 0;
		long int sum = 0;

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
			std::cout << "Unordered: " << allPatterns.size() << " Ordered: " << ordered_patterns.size() << std::endl;

			std::vector<long int> added_patterns;
			for(auto pattern: ordered_patterns)
			{
				std::cout << "Processing " << pattern.tostring(cci.classDecoder) << std::endl;

				Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern, 0, n-1);
				if(prefix != previousPrefix)
				{
					std::cout << "\tNew prefix! " << prefix.tostring(cci.classDecoder) << std::endl;
					std::cout << "\t\tFound " << added_patterns.size() << " elements for the old prefix: " << previousPrefix.tostring(cci.classDecoder) << std::endl;
					for(auto count : added_patterns)
					{
						double mle = count*1.0/sum;
						std::cout << "\t\tWith count: " << count << std::endl;
						llh -= log(mle);
					}
					std::cout << "\t\tIts llh is then: " << llh << "(sum=" << sum << ")" << std::endl;

					mleCounts[previousPrefix] = llh;

					llh = 0;
					sum = 0;
					added_patterns = std::vector<long int>();
					previousPrefix = prefix;
				}

				long int count;
				if(patternCounts)
				{
					patternCounts->get(pattern);
				} else
				{
					cci.trainPatternModel.occurrencecount(pattern);
				}
				sum += count;
				added_patterns.push_back(count);
			}
		}

		V = get(Pattern());
	}

};



/**
 * For each context we store by how many focus words it can be followed
 * (according to the train data)
 */
class ContextCounts
{

public:
	std::unordered_map<Pattern, long int> contextCounts;
	long int V = 0;

	void fromFile(SNCBWCoCoInitialiser& cci, const std::string& fileName)
	{

		std::ifstream file(fileName);
		std::string   line;

		std::set<Pattern, PatternComp> orderedPatterns;

		const bool allowUnknown = false;
		const bool autoAddUnknown = false;

		while(std::getline(file, line))
		{
		    std::stringstream   linestream(line);
		    std::string         patternString;
		    long int            patternCount;

		    std::getline(linestream, patternString, '\t');

		    Pattern pattern = cci.classEncoder.buildpattern(patternString, allowUnknown, autoAddUnknown);
//		    std::cout << fileName << "\tP:" << pattern.tostring(cci.classDecoder) << std::endl;

		    orderedPatterns.insert(pattern);
		}
		std::cout << "Done ordering the set" << std::endl;

		count(orderedPatterns);

		V = get(Pattern());
	}

	long int get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern,long int>::const_iterator iter = contextCounts.find(pattern);

		  if ( iter == contextCounts.end() )
		    return 0;
		  else
		    return iter->second;
	}

	void fromData(SNCBWCoCoInitialiser& cci)
	{
		for(int n = 1; n <= 4; ++n)
		{
			PatternSet<uint64_t> allPatterns = cci.trainPatternModel.extractset(n,n);
			std::cout << "Done extracting set for " << n << std::endl;

			std::set<Pattern, PatternComp> orderedPatterns;

			for(auto pattern : allPatterns)
			{
				orderedPatterns.insert(pattern);
			}
			std::cout << "Done ordering the set" << std::endl;

			count(orderedPatterns);
		}

		V = get(Pattern());
	}

	void count(const std::set<Pattern, PatternComp>& orderedPatterns)
	{
		Pattern previousPrefix = Pattern();
		int wordsPerContext = 0 ;

		std::vector<int> added_patterns = std::vector<int>();
		for(auto pattern: orderedPatterns)
		{
			Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern, 0, pattern.size()-1);
			if(prefix != previousPrefix)
			{
				contextCounts[previousPrefix] = wordsPerContext;

				wordsPerContext = 0;
				previousPrefix = prefix;
			}

			++wordsPerContext;
		}
	}

};


#endif /* HPYPLM_CONTEXTVALUES_H_ */
