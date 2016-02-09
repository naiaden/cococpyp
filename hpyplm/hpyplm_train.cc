#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"
#include "cpyp/m.h"
#include "cpyp/random.h"
#include "cpyp/crp.h"
#include "cpyp/tied_parameter_resampler.h"

#include "cpyp/boost_serializers.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <map>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <sstream>
#include <iomanip>

#include "hpyplm/uniform_vocab.h"
#include "cmdline.h"
#include "DefaultPatternModelOptions.h"
#include "ProgramOptions.h"
#include "CoCoInitialiser.h"
#include "utils.h"

#include "date.h"

using date::operator<<;

int main(int argc, char** argv) {
    cpyp::MT19937 _eng(1234);

    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();

    TrainCommandLineOptions tclo(argc, argv);
    TrainProgramOptions po(tclo, std::stoi(_kORDER));
    PatternModelOptions pmo = TrainPatternModelOptions(tclo, std::stoi(_kORDER)).patternModelOptions;
    CoCoInitialiser cci = CoCoInitialiser(po, pmo, true, true);
    cci.printStats(std::stoi(_kORDER));
    
    std::string moutputFile(po.generalBaseOutputName + ".output");
    my_ostream mout(moutputFile);
    
    mout << "Initialisation done at " << std::chrono::system_clock::now() << std::endl;
    mout << "Running on " << po.hostName << std::endl;

    TimeStatsPrinter tsp(cci.trainPatternModel.totalpatternsingroup(0,4));

    std::cout << "# word types: " << cci.trainPatternModel.totalwordtypesingroup(0,0) << std::endl;
    std::cout << "<" << pmo.MINLENGTH << "," << pmo.MAXLENGTH << ">" << std::endl;
    cpyp::PYPLM<kORDER> lm(cci.trainPatternModel.totalwordtypesingroup(0, 0), 1, 1, 1, 1);

    for(int sample = 0; sample < po.samples; ++sample) 
    {

        tsp.nextSample();
        for(IndexedCorpus::iterator iter = cci.indexedCorpus->begin(); iter != cci.indexedCorpus->end(); ++iter)
        {
            for(PatternPointer pattern : cci.trainPatternModel.getreverseindex(iter.index(), 0, 0, 4))
            {
                std::cout << pattern.tostring(cci.classDecoder) << std::endl;
        
     //           tsp.printTimeStats();

                PatternPointer context = PatternPointer();
                PatternPointer focus = PatternPointer();

                context = PatternPointer(pattern, 0, std::stoi(_kORDER) - 1);
                focus = PatternPointer(pattern, std::stoi(_kORDER)-1, 1);

                if(sample > 0) 
                {
                    lm.decrement(focus, context, _eng);
                }
                lm.increment(focus, context, _eng, &cci.classDecoder);
//              std::cout << "Adding: " << context.tostring(_class_decoder) << " " << focus.tostring(_class_decoder) << std::endl;

            }
        }

/*
        tsp.nextSample();
        for(int i = 1; i < cci.indexedCorpus->sentences(); ++i)                     // sentences
        {
            PatternPointer sentencePointer = cci.indexedCorpus->getsentence(i);
            std::vector<PatternPointer> patterns;
            sentencePointer.ngrams(patterns, std::stoi(_kORDER));
            for(auto pattern : patterns)                                            // *grams
            {
                std::cout << pattern.tostring(cci.classDecoder) << std::endl;
        
     //           tsp.printTimeStats();

                PatternPointer context = PatternPointer();
                PatternPointer focus = PatternPointer();

                context = PatternPointer(pattern, 0, std::stoi(_kORDER) - 1);
                focus = PatternPointer(pattern, std::stoi(_kORDER)-1, 1);

                if(sample > 0) 
                {
                    lm.decrement(focus, context, _eng);
                }
                lm.increment(focus, context, _eng, &cci.classDecoder);
//              std::cout << "Adding: " << context.tostring(_class_decoder) << " " << focus.tostring(_class_decoder) << std::endl;
            }
        }
*/
        if(sample % 10 == 9) 
        {
            std::cout << std::endl;
            mout << " [LLH=" << lm.log_likelihood() << "]" << std::endl;
            if(sample % 30u == 29) 
            {
                lm.resample_hyperparameters(_eng);
            } else 
            {
                //
            }
        }
    }

    mout << "Sampling done at " << std::chrono::system_clock::now() << std::endl;

    std::ofstream ofile(po.trainSerialisedFileName, std::ios::binary);
    if (!ofile.good()) 
    {
        std::cerr << "Failed to open " << po.trainSerialisedFileName << " for writing" << std::endl;
        return 1;
    }

    boost::archive::binary_oarchive oa(ofile);
    oa << lm;

    mout << "Saving to file done at " << std::chrono::system_clock::now() << std::endl;

}

