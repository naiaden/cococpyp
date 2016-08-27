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
#include "common.h"

class ContextValues
{
public:
	virtual double get(const Pattern& pattern, const Pattern& w,CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const = 0;
	virtual double get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) const = 0;
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
		patternCounts[Pattern()] = 0;

//		for(int i = 1; i <= kORDER; ++i)
		for(int i = 1; i <= 5; ++i)
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

				try {
					Pattern pattern = cci.classEncoder.buildpattern(patternString, allowUnknown, autoAddUnknown);

					patternCounts[pattern] = patternCount;

					Pattern smallerPattern = (i==1) ? Pattern() : Pattern(pattern, 0, i-1);
					Pattern skipPattern = smallerPattern + cci.classEncoder.buildpattern("{*}", allowUnknown, autoAddUnknown);
					auto it = patternCounts.find(skipPattern);
					if(it != patternCounts.end())
					{
						it->second += patternCount;
					}
					else
					{
						patternCounts[skipPattern] = patternCount;
					}

				} catch (const UnknownTokenError &e) {
					//
				}
			}
		}

	}

	long int get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) const
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

	double get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) const
	{
		return 1.0;
	}

	double get(const Pattern& pattern, const Pattern& w, CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
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
		Pattern skip = cci.classEncoder.buildpattern("{*}", false, false);

		for(int n = 1; n <= kORDER; ++n)
		{
			std::cout << "Extracting MLE set for " << n << "...";
			PatternSet<uint64_t> allPatterns = cci.trainPatternModel.extractset(n,n);
			std::cout << " done" << std::endl;

			long int countAllUnigrams = 0;
			if(n==1)
			{
				for(auto pattern: allPatterns)
				{
					countAllUnigrams += 1;
				}
				mleCounts[Pattern()] = 1.0/countAllUnigrams;
			}

			for(auto pattern : allPatterns)
			{
				Pattern context = (n==1) ? Pattern() : Pattern(pattern, 0, n-1);

				long int contextCount = 0;
				if(n==1)
				{
					contextCount = countAllUnigrams;
				} else
				{
					contextCount = patternCounts->get(context);
				}

				if(contextCount > 0)
				{
					mleCounts[pattern] = 1.0 * patternCounts->get(pattern) / contextCount;
				} else
				{
					mleCounts[pattern] = CoCoInitialiser::epsilon;
				}
			}
		}
	}

	double get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) const
	{
		std::unordered_map<Pattern, double>::const_iterator iter = mleCounts.find(pattern);
		if ( iter != mleCounts.end() )
		{
			return iter->second;
		} else
		{
			return CoCoInitialiser::epsilon;//iter->second;
		}

	}

	double get(const Pattern& context, const Pattern& w, CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
	{
		return get(context + w, cci);
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

	double emptyEntropy = 1.0;
	long int V = 0;

	EntropyCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts = nullptr)
	{
		initialise(cci, patternCounts);
		emptyEntropy = get(Pattern());
	}


	double get(const Pattern& context,
			CoCoInitialiser * const cci = nullptr) const
	{
		if (context == cci->classEncoder.buildpattern("{*}", false, false))
		{
			return emptyEntropy;
		}

		if (context == Pattern())
		{
			return emptyEntropy;
		}

		std::unordered_map<Pattern, double>::const_iterator iter = entropyCounts.find(context);

		if (iter != entropyCounts.end())
		{
			double rv = 1.0 + std::abs(1.0 / (1.0 - iter->second) - 2);
			return rv;
		} else
		{
			return emptyEntropy;
		}
	}

	// FIX
	double get(const Pattern& context, const Pattern& w, CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
	{
		return get(context, cci);
	}



	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
	{
		Pattern previousPrefix = Pattern();
		double llh = 0;
		long int sum = 0;

		std::vector<long int> added_patterns;
		for(int n = 1; n <= kORDER; ++n)
		{
			PatternSet<uint64_t> allPatterns = cci.trainPatternModel.extractset(n,n);
			std::cout << "Done extracting entropy set for " << n << std::endl;

			std::set<Pattern, PatternComp> ordered_patterns;

			for(auto pattern : allPatterns)
			{
				ordered_patterns.insert(pattern);
			}

			//


			for(auto pattern: ordered_patterns)
			{
				Pattern context = pattern.size() == 1 ? Pattern() : Pattern(pattern, 0, n-1);
				if(context != previousPrefix)
				{
					double entropySum = 0;
					for(auto count : added_patterns)
					{
						double mle = 1.0*count/sum;
						entropySum += mle * log(mle);
					}

//					std::string::size_type n = previousPrefix.tostring(cci.classDecoder).find("justify");
//					if(n != std::string::npos)
//					{
//						std::cout << "\t\tFound " << added_patterns.size() << " elements"
//								<< " for the context: " << previousPrefix.tostring(cci.classDecoder)
//								<< " with sum: " << sum
//								<< " resulting in entropy: " << -entropySum
//								<<   std::endl;
//					}

					entropyCounts[previousPrefix] = -entropySum;

					sum = 0;
					added_patterns = std::vector<long int>();
					previousPrefix = context;
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
	Pattern oneSkipPattern = Pattern();

	// The empty pattern can be followed by any word, so get(Pattern()) = V
	//
	void fromFile(SNCBWCoCoInitialiser& cci)
	{
		for(int i = 1; i <= kORDER; ++i)
		{
			SNCBWProgramOptions* spo = (SNCBWProgramOptions*) cci.po;
			std::ifstream file(spo->countFilesBase + "." + std::to_string(i));
			std::string   line;

			std::set<Pattern> orderedPatterns;

			const bool allowUnknown = false;
			const bool autoAddUnknown = false;


			while(std::getline(file, line))
			{
				std::stringstream   linestream(line);
				std::string         patternString;
				long int            patternCount;

				std::getline(linestream, patternString, '\t');

//				Pattern pattern = cci.classEncoder.buildpattern(patternString, allowUnknown, autoAddUnknown);
			    //std::cout << "P:" << pattern.tostring(cci.classDecoder) << "(" << pattern.unknown() << ")" << std::endl;


				try {
					Pattern pattern = cci.classEncoder.buildpattern(patternString, allowUnknown, autoAddUnknown);
					orderedPatterns.insert(pattern);
//					std::cout << "+P:" << pattern.tostring(cci.classDecoder) << std::endl;
				} catch (const UnknownTokenError &e) {
//					std::cout << "-P:" << patternString << std::endl;
				}




//			    if(!pattern.unknown())
//			    	orderedPatterns.insert(pattern);
			}
			std::cout << "Done ordering the set" << std::endl;

			count(orderedPatterns);

			if(i == 1)
			{
				V = orderedPatterns.size();//get(Pattern());
				oneSkipPattern = cci.classEncoder.buildpattern("{*}", allowUnknown, autoAddUnknown);
			}


		}
//		for(auto cc : contextCounts)
//		{
//			std::cout << "\"" << cc.first.tostring(cci.classDecoder) << "\"" << cc.second << std::endl;
//		}
	}

	long int get(const Pattern& pattern) const
	{
		if(pattern == Pattern())
		  {
			 return V;
		  }

		if(pattern == oneSkipPattern)
		  {
			 return V;
		  }

		std::unordered_map<Pattern,long int>::const_iterator iter = contextCounts.find(pattern);

		  if ( iter == contextCounts.end() )
		  {
		    return 0;
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

			std::set<Pattern> orderedPatterns;

			for(auto pattern : allPatterns)
			{
				orderedPatterns.insert(pattern);
			}
			std::cout << "Done ordering the set" << std::endl;

			count(orderedPatterns);
		}

		V = get(Pattern());
	}

	void count(const std::set<Pattern>& orderedPatterns)
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
