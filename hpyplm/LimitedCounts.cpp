/*
 * LimitedCounts.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */

#include "LimitedCounts.h"

#include "PatternCounts.h"
#include "strategies.h"




std::string LimitedCounts::name() const
{
	return "limitedcounts";
}

LimitedCounts::LimitedCounts(SNCBWCoCoInitialiser& cci, const std::string& fileName)
{
	std::cout << "Reading LimitedCounts from file: " << fileName << "...";
	std::ifstream file(fileName);
	std::string   line;

	std::set<Pattern> orderedPatterns;

	const bool allowUnknown = false;
	const bool autoAddUnknown = false;

	while(std::getline(file, line))
	{
		std::stringstream   linestream(line);
		std::string         patternString;
		std::string         PValueString;
		double              PValue;
		std::string			nobackoffString;
		double 				nobackoff;
		std::string			backoffString;
		double				backoff;

		std::getline(linestream, patternString, '\t');
		std::getline(linestream, PValueString, '\t');
		PValue = std::stod(PValueString);
		std::getline(linestream, nobackoffString, '\t');
		nobackoff = std::stod(nobackoffString);
		std::getline(linestream, backoffString, '\t');
		backoff = std::stod(backoffString);

//		std::cout << "[" << backoff << "] {" << nobackoff << "} (" << PValue  << ")" << std::endl;

		try {
			Pattern pattern = cci.classEncoder.buildpattern(patternString, allowUnknown, autoAddUnknown);

			LimitedInformation li;
			li.backoff = backoff;
			li.nobackoff = nobackoff;
			li.P = PValue;

			limitedCounts[pattern] = li;
		} catch (const UnknownTokenError &e) {
			//
		}
	}
	std::cout << " Done." << std::endl;

}

LimitedCounts::LimitedCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts, BackoffStrategy* _backoffStrategy)
{
	backoffStrategy = _backoffStrategy;
	initialise(cci, patternCounts);
}

void LimitedCounts::initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts)
{

	SNCBWProgramOptions* po = (SNCBWProgramOptions*) cci.po;
	std::ofstream probsFile;
	probsFile.open(po->generalLimitedCacheFileName + "." + backoffStrategy->strategyName());

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


		long int kounter = 0;
		std::cout << "Progress";
		for(auto context : allContexts)
		{
			++kounter;
			LimitedInformation li;
			long int nobackoff = 0;
			double P = 0.0;
			for(auto focus : allFocusWords)
			{

				Pattern pattern = context + focus;
				if(patternCounts->get(pattern))
				{ // not oov
					++nobackoff;
//					std::cout << "NOT OOV: " << pattern.tostring(cci.classDecoder) << std::endl;
					P += backoffStrategy->prob(focus, context, "" /*focus.tostring(cci.classDecoder)*/);
				}
			}

			li.nobackoff = nobackoff;
			li.backoff = numberOfFocusWords - nobackoff;
			li.P = P;

			probsFile << context.tostring(cci.classDecoder) << "\t" << P << "\t" << nobackoff << "\t" << li.backoff << "\n";

			if(kounter % 10 == 0) // std::setw(4) << (oov*1.0/counter*100) << "%
				std::cout << std::fixed << "\r" << kounter << ":" << std::setw(6) << kounter*1.0/allContexts.size()*100 << "%" << std::flush;
		}
	}
	std::cout << std::endl;
	probsFile.close();
	std::cout << "Done extracting LimitedCounts" << std::endl;
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

