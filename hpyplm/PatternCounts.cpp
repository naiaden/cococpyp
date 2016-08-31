/*
 * PatternCounts.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */


#include "PatternCounts.h"



inline void PatternCounts::fromFile(SNCBWCoCoInitialiser& cci)
{
	patternCounts[Pattern()] = 0;

	Pattern removeme = Pattern();

//		for(int i = 1; i <= kORDER; ++i)
	for(int i = 1; i <= 5; ++i)
	{
		SNCBWProgramOptions* spo = (SNCBWProgramOptions*) cci.po;
		std::ifstream file(spo->countFilesBase + "." + std::to_string(i));
		std::cout << "Reading pattern counts from " << spo->countFilesBase + "." + std::to_string(i) << std::endl;
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
				if(patternString.compare("<s> <s> <s> The") == 0)
					{
					std::cout << pattern.tostring(cci.classDecoder) << " -- " << patternCount << std::endl;
					std::cout << "retrieved count: " << get(pattern) << std::endl;
					removeme = pattern;
					}

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

	std::cout << "retrieved count': " << get(removeme) << std::endl;
}

inline long int PatternCounts::get(const Pattern& pattern, CoCoInitialiser * const cci) const
{
	std::unordered_map<Pattern,long int>::const_iterator iter = patternCounts.find(pattern);

	  if ( iter == patternCounts.end() )
		return 0;//0;
	  else
		return iter->second;//iter->second;
}


