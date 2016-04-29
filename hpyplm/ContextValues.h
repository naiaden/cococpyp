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
	virtual std::string name() const = 0;
};

/**
 * For each context (or pattern) we store how often it occurs
 * (according to the train data)
 */
class PatternCounts
{

public:
	std::unordered_map<Pattern, long int> patternCounts;

	void fromFile(SNCBWCoCoInitialiser& cci)
	{
		for(int i = 1; i <= kORDER; ++i)
		{
			SNCBWProgramOptions* spo = (SNCBWProgramOptions*) cci.po;
			std::ifstream file(spo->countFilesBase + "." + std::to_string(i));
			std::cout << "Reading countFile " << spo->countFilesBase + "." + std::to_string(i) << std::endl;
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
	//			std::cout << "P:" << pattern.tostring(cci.classDecoder) << " C:" << patternCount << "(" << patternCounts[pattern] << ")" << std::endl;
			}
		}

//		for(auto pc : patternCounts)
//		{
//			if(pc.second > 1)
//			std::cout << "\"" << pc.first.tostring(cci.classDecoder) << "\"" << pc.second << std::endl;
//		}
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
	std::string name() const
	{
		return "uniform";
	}

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
	std::string name() const
	{
		return "mle";
	}

	std::unordered_map<Pattern, double> mleCounts;

	MLECounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
	{
		initialise(cci, patternCounts);
	}

	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
	{
		for(int n = 1; n <= kORDER; ++n)
		{
			PatternSet<uint64_t> allPatterns = cci.trainPatternModel.extractset(n,n);
			std::cout << "Done extracting set for " << n << std::endl;

			long int countAllUnigrams = 0;
			if(n==1)
			{
				for(auto pattern: allPatterns)
				{
					countAllUnigrams += patternCounts->get(pattern);
				}
				mleCounts[Pattern()] = countAllUnigrams;
			}

			for(auto pattern : allPatterns)
			{
				Pattern smallerPattern = (n==1) ? Pattern() : Pattern(pattern, 0, n-1);

				long int smallerCount = 0;
				if(n==1)
				{
					smallerCount = countAllUnigrams;
				} else
				{

					smallerCount = patternCounts->get(smallerPattern);
				}

//				std::cout << "\"" << pattern.tostring(cci.classDecoder) << "\"\t"
//						<< " c(" << pattern.tostring(cci.classDecoder) << ") = " << patternCounts->get(pattern)
//						<< " / c(" << smallerPattern.tostring(cci.classDecoder) << ") = " << smallerCount
//						<< "\t= " << 1.0 * patternCounts->get(pattern) / smallerCount
//						<< std::endl;
				mleCounts[pattern] = 1.0 * patternCounts->get(pattern) / smallerCount;
			}
		}
	}

	double get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern, double>::const_iterator iter = mleCounts.find(pattern);
		if ( iter != mleCounts.end() )
			return iter->second;
		else
			return 0;//iter->second;
	}
};

class EntropyCounts : public ContextValues
{
	public:
	std::string name() const
	{
		return "entropy";
	}

	std::unordered_map<Pattern, double> entropyCounts;

	long int V = 0;

	EntropyCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts = nullptr)
	{
		initialise(cci, patternCounts);
		for (auto mc: entropyCounts)
		{
			std::cout << "\"" << mc.first.tostring(cci.classDecoder) << "\"" << mc.second << std::endl;
		}
	}


	// FIX
	double get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern, double>::const_iterator iter = entropyCounts.find(pattern);

		  if ( iter != entropyCounts.end() )
		    return 1.0/iter->second;
		  else
		    return 1.0;//iter->second;
	}



	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
	{
		Pattern previousPrefix = Pattern();
		double llh = 0;
		long int sum = 0;

		for(int n = 1; n <= kORDER; ++n)
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

			//

			std::vector<long int> added_patterns;
			for(auto pattern: ordered_patterns)
			{
				Pattern prefix = pattern.size() == 1 ? Pattern() : Pattern(pattern, 0, n-1);
				if(prefix != previousPrefix)
				{
					double entropySum = 0;
					for(auto count : added_patterns)
					{
						double mle = 1.0*count/sum;
						entropySum += mle * log(mle);
					}

//					std::cout << "\t\tFound " << added_patterns.size() << " elements"
//							<< " for the prefix: " << previousPrefix.tostring(cci.classDecoder)
//							<< " with sum: " << sum
//							<< " resulting in entropy: " << -entropySum
//							<<   std::endl;

					entropyCounts[previousPrefix] = -entropySum;

					sum = 0;
					added_patterns = std::vector<long int>();
					previousPrefix = prefix;
				}

				long int count = patternCounts->get(pattern);

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

	// The empty pattern can be followed by any word, so get(Pattern()) = V
	//
	void fromFile(SNCBWCoCoInitialiser& cci)
	{
		for(int i = 1; i <= kORDER; ++i)
		{
			SNCBWProgramOptions* spo = (SNCBWProgramOptions*) cci.po;
			std::ifstream file(spo->countFilesBase + "." + std::to_string(i));
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

			if(i == 1)
			{
				V = orderedPatterns.size();//get(Pattern());
			}


		}
//		for(auto cc : contextCounts)
//		{
//			std::cout << "\"" << cc.first.tostring(cci.classDecoder) << "\"" << cc.second << std::endl;
//		}
	}

	long int get(const Pattern& pattern) const
	{
		std::unordered_map<Pattern,long int>::const_iterator iter = contextCounts.find(pattern);

		  if ( iter == contextCounts.end() )
		  {
		    return 0;
		  } else if(pattern == Pattern())
		  {
			 return V;
		  } else
		  {
		    return iter->second;
		  }
	}

	void fromData(SNCBWCoCoInitialiser& cci)
	{
		for(int n = 1; n <= kORDER; ++n)
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
