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
//    EntropyCounts entropyCounts(cci, &patternCounts);
//    UniformCounts uniformCounts(cci);

    Pattern h = cci.classEncoder.buildpattern("{*}", false, false);
    std::cout << "H: " << h.tostring(cci.classDecoder) << std::endl;

    Pattern g = cci.classEncoder.buildpattern("of the common", false, false);
    std::cout << "G: " << Pattern(g, 0, 0).tostring(cci.classDecoder) << std::endl;
	std::cout << "G: " << Pattern(g, 0, 1).tostring(cci.classDecoder) << std::endl; // of
	std::cout << "G: " << Pattern(g, 0, 2).tostring(cci.classDecoder) << std::endl; // of the
	std::cout << "G: " << Pattern(g, 0, 3).tostring(cci.classDecoder) << std::endl; // of the common
	std::cout << "G: " << Pattern(g, 0, 4).tostring(cci.classDecoder) << std::endl; // of the common
	std::cout << "G: " << Pattern(g, 1, 2).tostring(cci.classDecoder) << std::endl; // the common
	std::cout << "G: " << Pattern(g, 1, 2).tostring(cci.classDecoder) << std::endl; // the common
	std::cout << "G: " << Pattern(g, 1, 3).tostring(cci.classDecoder) << std::endl; // the common
	std::cout << "G: " << Pattern(g, 2, 3).tostring(cci.classDecoder) << std::endl; // common

    Pattern g1 = cci.classEncoder.buildpattern("of the common", false, false);
    std::cout << "G1: " << g1.tostring(cci.classDecoder) << std::endl;

    Pattern g1h = cci.classEncoder.buildpattern("of the", false, false);
    Pattern g1w = cci.classEncoder.buildpattern("common", false, false);
    std::cout << "\t" << g1.tostring(cci.classDecoder) << "\t" << patternCounts.get(g1) << std::endl;
    std::cout << "\t" << g1h.tostring(cci.classDecoder) << "\t" << patternCounts.get(g1h) << std::endl;
    std::cout << "\t" << g1w.tostring(cci.classDecoder) << "\t" << patternCounts.get(g1w) << std::endl;
    mleCounts.get(g1h, g1w, &cci);


	Pattern g2 = cci.classEncoder.buildpattern("of", false, false);
	std::cout << "G2: " << g2.tostring(cci.classDecoder) << std::endl;

	Pattern g2h = cci.classEncoder.buildpattern("", false, false);
	Pattern g2w = cci.classEncoder.buildpattern("of", false, false);
    std::cout << "\t" << g2.tostring(cci.classDecoder) << "\t" << patternCounts.get(g2) << std::endl;
    std::cout << "\t" << g2h.tostring(cci.classDecoder) << "\t" << patternCounts.get(g2h) << std::endl;
    std::cout << "\t" << g2w.tostring(cci.classDecoder) << "\t" << patternCounts.get(g2w) << std::endl;
	mleCounts.get(g2h, g2w, &cci);


	Pattern g3 = cci.classEncoder.buildpattern("of {*} common", false, false);
	std::cout << "G3: " << g3.tostring(cci.classDecoder) << std::endl;

	Pattern g3w = cci.classEncoder.buildpattern("common", false, false);
	Pattern g3h = cci.classEncoder.buildpattern("of {*}", false, false);
    std::cout << "\t" << g3.tostring(cci.classDecoder) << "\t" << patternCounts.get(g3) << std::endl;
    std::cout << "\t" << g3h.tostring(cci.classDecoder) << "\t" << patternCounts.get(g3h) << std::endl;
    std::cout << "\t" << g3w.tostring(cci.classDecoder) << "\t" << patternCounts.get(g3w) << std::endl;
	mleCounts.get(g3h, g3w, &cci);


	Pattern g4 = cci.classEncoder.buildpattern("rules of the common", false, false);
	std::cout << "G4: " << g4.tostring(cci.classDecoder) << std::endl;

	Pattern g4w = cci.classEncoder.buildpattern("common", false, false);
	Pattern g4h = cci.classEncoder.buildpattern("rules of the", false, false);
    std::cout << "\t" << g4.tostring(cci.classDecoder) << "\t" << patternCounts.get(g4) << std::endl;
    std::cout << "\t" << g4h.tostring(cci.classDecoder) << "\t" << patternCounts.get(g4h) << std::endl;
    std::cout << "\t" << g4w.tostring(cci.classDecoder) << "\t" << patternCounts.get(g4w) << std::endl;
	mleCounts.get(g4h, g4w, &cci);


	Pattern g5 = cci.classEncoder.buildpattern("rules {*} {*} common", false, false);
	std::cout << "G5: " << g5.tostring(cci.classDecoder) << std::endl;

	Pattern g5w = cci.classEncoder.buildpattern("common", false, false);
	Pattern g5h = cci.classEncoder.buildpattern("rules {*} {*}", false, false);
    std::cout << "\t" << g5.tostring(cci.classDecoder) << "\t" << patternCounts.get(g5) << std::endl;
    std::cout << "\t" << g5h.tostring(cci.classDecoder) << "\t" << patternCounts.get(g5h) << std::endl;
    std::cout << "\t" << g5w.tostring(cci.classDecoder) << "\t" << patternCounts.get(g5w) << std::endl;
	mleCounts.get(g5h, g5w, &cci);


	Pattern g6 = cci.classEncoder.buildpattern("of the", false, false);
	std::cout << "G6: " << g6.tostring(cci.classDecoder) << std::endl;

	Pattern g6w = cci.classEncoder.buildpattern("the", false, false);
	Pattern g6h = cci.classEncoder.buildpattern("of", false, false);
    std::cout << "\t" << g6.tostring(cci.classDecoder) << "\t" << patternCounts.get(g6) << std::endl;
    std::cout << "\t" << g6h.tostring(cci.classDecoder) << "\t" << patternCounts.get(g6h) << std::endl;
    std::cout << "\t" << g6w.tostring(cci.classDecoder) << "\t" << patternCounts.get(g6w) << std::endl;
	mleCounts.get(g6h, g6w, &cci);


	Pattern g7 = cci.classEncoder.buildpattern("brr {*} {*} common", false, false);
	std::cout << "G7: " << g7.tostring(cci.classDecoder) << std::endl;

	Pattern g7w = cci.classEncoder.buildpattern("common", false, false);
	Pattern g7h = cci.classEncoder.buildpattern("rules {*} {*}", false, false);
	std::cout << "\t" << g7.tostring(cci.classDecoder) << "\t" << patternCounts.get(g7) << std::endl;
	std::cout << "\t" << g7h.tostring(cci.classDecoder) << "\t" << patternCounts.get(g7h) << std::endl;
	std::cout << "\t" << g7w.tostring(cci.classDecoder) << "\t" << patternCounts.get(g7w) << std::endl;
	mleCounts.get(g7h, g7w, &cci);

//    exit(1);

    BackoffStrategies backoffStrategies;
//    backoffStrategies.addBackoffStrategy(new NgramBackoffStrategy(po, cci, lm));
    backoffStrategies.addBackoffStrategy(new LimitedBackoffStrategy(po, cci, lm, &contextCounts, &mleCounts));
//    backoffStrategies.addBackoffStrategy(new FullBackoffStrategy(po, cci, lm, &mleCounts));
//    backoffStrategies.addBackoffStrategy(new LimitedBackoffStrategy(po, cci, lm, &contextCounts, &uniformCounts));
//    backoffStrategies.addBackoffStrategy(new FullBackoffStrategy(po, cci, lm, &uniformCounts));
    
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



