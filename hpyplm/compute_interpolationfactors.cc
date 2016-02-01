#include <iostream>
#include <unordered_map>

#include <vector>
#include <map>
#include <set>

#include <sstream>
#include <iomanip>


#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include "cmdline.h"
#include "DefaultPatternModelOptions.h"
#include "ProgramOptions.h"
#include "CoCoInitialiser.h"

struct PatternComp
{
    bool operator() (const Pattern& lhs, const Pattern& rhs) const
    {
        return lhs>rhs;
    }
};

int main(int argc, char** argv) 
{
    SNCBWCommandLineOptions clo = SNCBWCommandLineOptions(argc, argv);
    SNCBWProgramOptions po = SNCBWProgramOptions(clo);
    DefaultPatternModelOptions dpmo = DefaultPatternModelOptions();

    CoCoInitialiser cci = CoCoInitialiser(po, dpmo, true);


   std::ofstream _general_output;
   _general_output.open(po.getGeneralInterpolationFactorsOutputName());


    for(int n = 1; n <= 4; ++n)
    {
        PatternSet<uint64_t> allPatterns = cci.getTrainPatternModel().extractset(n,n);
        std::cout << "Done extracting set for " << n << std::endl;
        
        std::set<Pattern, PatternComp> ordered_patterns;

        for(auto pattern : allPatterns)
        {
            ordered_patterns.insert(pattern);
        }
        std::cout << "Done ordering the set" << std::endl;

        Pattern previous_prefix = Pattern();
        double llh = 0;
        int sum = 0;
        std::vector<int> added_patterns = std::vector<int>();
        for(auto pattern: ordered_patterns)
        {
            Pattern prefix = Pattern(pattern, 0, n-1);
            if(prefix != previous_prefix)
            {
                for(auto count : added_patterns)
                {
                    double mle = count*1.0/sum;
                    llh -= log(mle);
                }
    
                std::cout << previous_prefix.tostring(cci.getClassDecoder()) << "\t" << added_patterns.size() << "\t" << -llh << "\t" << llh/added_patterns.size() << std::endl;
//                _general_output << previous_prefix.tostring(cci.getClassDecoder()) << "\t" << added_patterns.size() << "\t" << -llh << "\t" << llh/added_patterns.size() << std::endl;
                previous_prefix = prefix;

                llh = 0;
                sum = 0;
                added_patterns = std::vector<int>();
            }
            
            int count = cci.getTrainPatternModel().occurrencecount(pattern);
            sum += count;
            added_patterns.push_back(count);
        }
    }

   _general_output.close();
}
