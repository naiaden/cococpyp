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

int main(int argc, char** argv) {
    cpyp::MT19937 _eng(1234);

    bool _ignore_errors = true;

    std::cout << "Started at " << giveTime() << std::endl;

    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();


    TrainCommandLineOptions tclo(argc, argv);
    TrainProgramOptions po(tclo, std::stoi(_kORDER));
    PatternModelOptions pmo = TrainPatternModelOptions(tclo, std::stoi(_kORDER)).patternModelOptions;
    CoCoInitialiser cci = CoCoInitialiser(po, pmo, true, true);
    cci.printStats(std::stoi(_kORDER));
    
    std::ofstream _output;
    std::string outputFilename = po.generalBaseOutputName + ".output";
    _output.open(outputFilename);
    
    std::cout << "Running on " << po.hostName << std::endl;


    cpyp::PYPLM<kORDER> lm(cci.trainPatternModel.totalwordtypesingroup(0, 0), 1, 1, 1, 1);
    for(int sample = 0; sample < po.samples; ++sample) 
    {
        int patternsProcessed = 0;
        int patternsUnprocessed = 0;

        size_t icSize = cci.indexedCorpus->size();
        size_t counter = 0;
        for(IndexedCorpus::iterator iter = cci.indexedCorpus->begin(); iter != cci.indexedCorpus->end(); ++iter)
        {
            std::cout << "\r" << "Sample [" << sample << "]\tPattern: " << counter;
            
            Pattern pattern = iter.pattern();
        
            if(cci.trainPatternModel.has(pattern)) 
            {
                size_t patternSize = pattern.size();

                Pattern context = Pattern();
                Pattern focus = Pattern();

                if(patternSize == 4) // kORDER
                {
                    if(patternSize == 1) 
                    {
                        focus = pattern[0];
                    } else 
                    {
                        context = Pattern(pattern, 0, patternSize - 1);
                        focus = pattern[patternSize - 1];
                        ++patternsProcessed;
                    }

                    if(sample > 0) 
                    {
                        lm.decrement(focus, context, _eng);
                    }
                    lm.increment(focus, context, _eng, nullptr);
//                      std::cout << "Adding: " << context.tostring(_class_decoder) << " " << focus.tostring(_class_decoder) << std::endl;
                }
            }
        }

        if(sample % 10 == 9) 
        {
            std::cerr << " [LLH=" << lm.log_likelihood() << "]" << std::endl;
            if(sample % 30u == 29) 
            {
                lm.resample_hyperparameters(_eng);
            } else 
            {
                std::cerr<< ".";
            }
        }
    }


    std::ofstream ofile(po.trainSerialisedFileName, std::ios::binary);
    if (!ofile.good()) 
    {
        std::cout << "Failed to open " << po.trainSerialisedFileName << " for writing" << std::endl;
        return 1;
    }

    boost::archive::binary_oarchive oa(ofile);
    oa << lm;

    _output.close();
}

