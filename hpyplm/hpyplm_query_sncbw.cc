#include <iostream>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"

#include "cpyp/boost_serializers.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <map>
#include <set>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <sstream>
#include <iomanip>

#include <queue>

#include "cmdline.h"
#include "DefaultPatternModelOptions.h"
#include "ProgramOptions.h"
#include "CoCoInitialiser.h"

#include "utils.h"
#include "date.h"
#include "strategies.h"

using date::operator<<;

int main(int argc, char** argv) {
    std::cout << "Started at " << std::chrono::system_clock::now()  << std::endl;
    
    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();

    SNCBWCommandLineOptions qclo = SNCBWCommandLineOptions(argc, argv);
    std::cout << "Loaded QCLO" << std::endl;
    SNCBWProgramOptions po = SNCBWProgramOptions(qclo, std::stoi(_kORDER));
    std::cout << "Loaded PO" << std::endl;
    PatternModelOptions pmo = DefaultPatternModelOptions(false, kORDER).patternModelOptions;
    std::cout << "Loaded PMO" << std::endl;

    SNCBWCoCoInitialiser cci(po, pmo, true);
    std::cout << "Loaded CCI" << std::endl;


    std::string moutputFile(po.generalBaseOutputName + ".output");
    my_ostream mout(moutputFile);

    mout << "Initialisation done at " << std::chrono::system_clock::now() << std::endl;


    std::ifstream ifs(po.trainSerialisedFileName, std::ios::binary);
    if(!ifs.good()) {
        std::cerr << "Something went wrong whilst reading the model: " << po.trainSerialisedFileName << std::endl;
    }
    boost::archive::binary_iarchive ia(ifs);

    cpyp::PYPLM<kORDER> lm;
    ia & lm;
    mout << "Loaded serialised model" << std::endl;

/////////////////////////////////////////////////

    PatternSet<uint64_t> allWords = cci.trainPatternModel.extractset(1,1);
    mout << "Extracted all words" << std::endl;

    mout << "Preparation done at " << std::chrono::system_clock::now() << std::endl;


    QueryTimeStatsPrinter tsp(&mout); 
    tsp.start();

    ContextCounts contextCounts;
    contextCounts.fromFile(cci);

	PatternCounts patternCounts;
	patternCounts.fromFile(cci);

    MLECounts mleCounts(cci, &patternCounts);
    EntropyCounts entropyCounts(cci, &patternCounts);
    UniformCounts uniformCounts(cci);



    BackoffStrategies backoffStrategies;
    backoffStrategies.addBackoffStrategy(new NgramBackoffStrategy(po, cci, lm));

//    backoffStrategies.addBackoffStrategy(new LimitedBackoffStrategy(po, cci, lm, &patternCounts, &contextCounts, &mleCounts));
//    backoffStrategies.addBackoffStrategy(new FullBackoffStrategy(po, cci, lm, &contextCounts, &mleCounts));
//
//    backoffStrategies.addBackoffStrategy(new LimitedBackoffStrategy(po, cci, lm, &patternCounts, &contextCounts, &uniformCounts));
//    backoffStrategies.addBackoffStrategy(new FullBackoffStrategy(po, cci, lm, &contextCounts, &uniformCounts));

//    backoffStrategies.addBackoffStrategy(new LimitedBackoffStrategy(po, cci, lm, &patternCounts, &contextCounts, &entropyCounts));
    backoffStrategies.addBackoffStrategy(new FullBackoffStrategy(po, cci, lm, &contextCounts, &entropyCounts));
    
    for(std::string inputFileName : po.testInputFiles)                          // files
    {
        std::cout << "> " << inputFileName << std::endl;
   
        backoffStrategies.nextFile();
        tsp.nextFile();
        
        std::ifstream file(inputFileName);
        std::string retrievedString;
        while(std::getline(file, retrievedString))                              // lines
        {
            backoffStrategies.nextLine();
            tsp.nextSentence();
            std::vector<std::string> words = split(retrievedString);

            if(words.size() >= po.n) // kORDER
            {
               for(int i = (kORDER - 1); i < words.size(); ++i)                 // ngrams
               {
                    std::stringstream contextStream;
                    contextStream << words[i-(kORDER-1)];

                    for(int ii = 1; ii < kORDER - 1; ++ii)
                    {
                        contextStream << " " << words[i-(kORDER-1)+ii];
                    }

                    Pattern context = cci.classEncoder.buildpattern(contextStream.str());
                    Pattern focus = cci.classEncoder.buildpattern(words[i]);

                    double lp = 0.0;
                    std::string focusString = "";
                    if(/*focus.size() > 0 &&*/ !allWords.has(focus)) // empty if oov
                    {
                        focusString = words[i];
                    }
//                    std::cout << (!focusString.empty() ? "OOOOOOOOOOOOOOOOOOOOOOOOOV": "") << std::endl;
                    tsp.printTimeStats(!focusString.empty());
//                    std::cout << "[" << words[i] << "-" << focus.tostring(cci.classDecoder)
//                              << " " << contextStream.str() << "-" << context.tostring(cci.classDecoder)
//                              << "] --> " << focusString << std::endl;
                    backoffStrategies.prob(focus, context, focusString);
               }
            }
        }
        tsp.done();
        backoffStrategies.printFileResults();
    }
    backoffStrategies.done();
    std::cout << "\n\n" << std::endl;
    backoffStrategies.printResults();
    

}



