#include "ContextValues.h"

#include "CoCoInitialiser.h"
#include "utils.h"

#include "strategies.h"

#include "PatternCounts.h"












std::string UniformCounts::name() const
{
	return "uniform";
}

UniformCounts::UniformCounts(SNCBWCoCoInitialiser& cci)
{
}

double UniformCounts::get(const Pattern& pattern, CoCoInitialiser * const cci) const
{
	return 1.0;
}









std::string MLECounts::name() const
{
	return "mle";
}

MLECounts::MLECounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
{
	initialise(cci, patternCounts);
}

void MLECounts::initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
{
	Pattern skip = cci.classEncoder.buildpattern("{*}", false, false);

	Debug::getInstance() << DebugLevel::ALL << "### PatternCounts size: " << patternCounts->patternCounts.size() << std::endl;
	Debug::getInstance() << DebugLevel::ALL << "### MLE Counts size: " << mleCounts.size() << std::endl;

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
	Debug::getInstance() << DebugLevel::ALL << "### MLE Counts size: " << mleCounts.size() << std::endl;
}

double MLECounts::get(const Pattern& pattern, CoCoInitialiser * const cci) const
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









std::string EntropyCounts::name() const
{
	return "entropy";
}

EntropyCounts::EntropyCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
{
	initialise(cci, patternCounts);
	emptyEntropy = get(Pattern());
}


double EntropyCounts::get(const Pattern& context,
		CoCoInitialiser * const cci) const
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



void EntropyCounts::initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
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










