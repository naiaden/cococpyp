#ifndef DEFAULTPATTERNMODELOPTIONS_H_
#define DEFAULTPATTERNMODELOPTIONS_H_

#include <patternmodel.h>
#include "ProgramOptions.h"

struct DefaultPatternModelOptions
{
    PatternModelOptions patternModelOptions = PatternModelOptions();
  
  DefaultPatternModelOptions(bool generateSkipgrams = false, int maxLength = 4)
   {
        patternModelOptions.MAXLENGTH = maxLength;
        patternModelOptions.MINLENGTH = 1;
        patternModelOptions.DOSKIPGRAMS = generateSkipgrams;
        patternModelOptions.DOSKIPGRAMS_EXHAUSTIVE = generateSkipgrams;
        patternModelOptions.DOREVERSEINDEX = true;
        patternModelOptions.QUIET = false;
        patternModelOptions.MINTOKENS = 1;
   }
};

struct TrainPatternModelOptions : public DefaultPatternModelOptions
{
    TrainPatternModelOptions(TrainCommandLineOptions tclo, int maxLength = 4)
        : DefaultPatternModelOptions(tclo.skipgrams, maxLength)
    {
        patternModelOptions.MINTOKENS = tclo.nThreshold;
        patternModelOptions.MINTOKENS_SKIPGRAMS = tclo.sThreshold;
        patternModelOptions.MINTOKENS_UNIGRAMS = tclo.uThreshold;
        patternModelOptions.PRUNENONSUBSUMED = tclo.pruneLevel;
    }
};


#endif

