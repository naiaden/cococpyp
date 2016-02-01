#ifndef DEFAULTPATTERNMODELOPTIONS_H_
#define DEFAULTPATTERNMODELOPTIONS_H_

#include <patternmodel.h>

struct DefaultPatternModelOptions
{
    PatternModelOptions patternModelOptions = PatternModelOptions();
  
  DefaultPatternModelOptions(bool generateSkipgrams = false, int maxLength = 4)
   {
        patternModelOptions.MAXLENGTH = maxLength;
        patternModelOptions.MINLENGTH = 1;
        patternModelOptions.DOSKIPGRAMS = generateSkipgrams;
        patternModelOptions.DOSKIPGRAMS_EXHAUSTIVE = false;
        patternModelOptions.DOREVERSEINDEX = true;
        patternModelOptions.QUIET = false;
        patternModelOptions.MINTOKENS = 1;
   }
};


#endif

