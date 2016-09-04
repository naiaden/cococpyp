/*
 * ContextCounts.cpp
 *
 *  Created on: Aug 29, 2016
 *      Author: louis
 */

#include "ContextCounts.h"

// The empty pattern can be followed by any word, so get(Pattern()) = V
//
void ContextCounts::fromFile(SNCBWCoCoInitialiser& cci)
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

long int ContextCounts::get(const Pattern& pattern) const
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

void ContextCounts::fromData(SNCBWCoCoInitialiser& cci)
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

void ContextCounts::count(const std::set<Pattern>& orderedPatterns)
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

