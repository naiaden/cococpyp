#include <iostream>
#include <unordered_map>
#include <cstdlib>



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
#include "date.h"
#include <queue>

#include "cmdline.h"
#include "DefaultPatternModelOptions.h"
#include "ProgramOptions.h"
#include "CoCoInitialiser.h"
#include "PatternCounts.h"
//
//#include "utils.h"

//
#include "hpyplm.h"

//#include "ContextCounts.h"
#include "ContextCounts.h"
#include "ContextValues.h"
#include "PatternCounts.h"
//
//

//#include "ContextValues.h"
#include "LimitedCounts.h"
#include "strategies.h"

#include "Debug.h"

#include "NbestList.h"

#include <iomanip>

using date::operator<<;

int main(int argc, char** argv) {
    std::cout << "Started at " << std::chrono::system_clock::now()  << std::endl;

    Debug::getInstance().setPrintTime(false);

    std::stringstream oss;
	oss << kORDER;
	std::string _kORDER = oss.str();

	SNCBWCommandLineOptions qclo = SNCBWCommandLineOptions(argc, argv);

	Debug::getInstance() << DebugLevel::ALL << "Loaded QCLO" << std::endl;
    SNCBWProgramOptions po = SNCBWProgramOptions(qclo, std::stoi(_kORDER));
    Debug::getInstance() << DebugLevel::ALL << "Loaded PO" << std::endl;
    PatternModelOptions pmo = DefaultPatternModelOptions(false, kORDER).patternModelOptions;
    Debug::getInstance() << DebugLevel::ALL << "Loaded PMO" << std::endl;

    SNCBWCoCoInitialiser cci(po, pmo, true);
    Debug::getInstance() << DebugLevel::ALL << "Loaded CCI" << std::endl;


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

//
//    QueryTimeStatsPrinter tsp(&mout);
//    tsp.start();
//
//    ContextCounts contextCounts;
//    contextCounts.fromFile(cci);
//
//	PatternCounts patternCounts;
//	patternCounts.fromFile(cci);
//
//    LimitedCountsCache* mleLimitedCounts = nullptr;
//	LimitedCountsCache* entropyLimitedCounts = nullptr;
//	LimitedCountsCache* uniformLimitedCounts = nullptr;
//
//
//	mout << "Initialising: " << backoffsToString(qclo.backoffMethod) << std::endl;
//    BackoffStrategies backoffStrategies;
//
//    if(backoffIn(Backoff::NGRAM, qclo.backoffMethod))
//    {
//    	backoffStrategies.addBackoffStrategy(new NgramBackoffStrategy(po, cci, lm));
//    }
//
//    MLECounts* mleCounts = nullptr;
//    if(backoffIn(Backoff::MLE, qclo.backoffMethod))
//    {
//    	mleCounts = new MLECounts(cci, &patternCounts);
//    	Debug::getInstance() << DebugLevel::ALL << "###: " << mleCounts->name() << std::endl;
//
//		if(backoffIn(Backoff::FULLMLE, qclo.backoffMethod))
//		{
//			backoffStrategies.addBackoffStrategy(new FullNaiveBackoffStrategy(po, cci, lm, &contextCounts, mleCounts));
//		}
//
//        if(backoffIn(Backoff::LIMMLE, qclo.backoffMethod))
//        {
//            if(po.limitedMLECacheFile.empty())
//            	mleLimitedCounts = new LimitedCountsCache(cci, &patternCounts, new BasicFullNaiveBackoffStrategy(po, cci, lm, &contextCounts, mleCounts));
//            else
//            	mleLimitedCounts = new LimitedCountsCache(cci, &patternCounts, po.limitedMLECacheFile, "mlec", new BasicFullNaiveBackoffStrategy(po, cci, lm, &contextCounts, mleCounts));
//
//            backoffStrategies.addBackoffStrategy(new LimitedNaiveBackoffStrategy(po, cci, lm, &patternCounts, &contextCounts, mleCounts, mleLimitedCounts));
//        }
//    }
//
//    EntropyCounts* entropyCounts = nullptr;
//	if(backoffIn(Backoff::ENT, qclo.backoffMethod))
//	{
//		entropyCounts = new EntropyCounts(cci, &patternCounts);
//
//		if(backoffIn(Backoff::FULLENT, qclo.backoffMethod))
//		{
//			backoffStrategies.addBackoffStrategy(new FullNaiveBackoffStrategy(po, cci, lm, &contextCounts, entropyCounts));
//		}
//
//		if(backoffIn(Backoff::LIMENT, qclo.backoffMethod))
//		{
//			if(po.limitedEntropyCacheFile.empty())
//				entropyLimitedCounts = new LimitedCountsCache(cci, &patternCounts, new BasicFullNaiveBackoffStrategy(po, cci, lm, &contextCounts, entropyCounts));
//			else
//				entropyLimitedCounts = new LimitedCountsCache(cci, &patternCounts, po.limitedEntropyCacheFile, "entc", new BasicFullNaiveBackoffStrategy(po, cci, lm, &contextCounts, entropyCounts));
//
//			backoffStrategies.addBackoffStrategy(new LimitedNaiveBackoffStrategy(po, cci, lm, &patternCounts, &contextCounts, entropyCounts, entropyLimitedCounts));
//		}
//	}
//
//	UniformCounts* uniformCounts = nullptr;
//	if(backoffIn(Backoff::UNI, qclo.backoffMethod))
//	{
//		uniformCounts = new UniformCounts(cci);
//
//		if(backoffIn(Backoff::FULLUNI, qclo.backoffMethod))
//		{
//			backoffStrategies.addBackoffStrategy(new FullNaiveBackoffStrategy(po, cci, lm, &contextCounts, uniformCounts));
//		}
//
//		if(backoffIn(Backoff::LIMUNI, qclo.backoffMethod))
//		{
//			if(po.limitedUniformCacheFile.empty())
//				uniformLimitedCounts = new LimitedCountsCache(cci, &patternCounts, new BasicFullNaiveBackoffStrategy(po, cci, lm, &contextCounts, uniformCounts));
//			else
//				uniformLimitedCounts = new LimitedCountsCache(cci, &patternCounts, po.limitedUniformCacheFile, "unic", new BasicFullNaiveBackoffStrategy(po, cci, lm, &contextCounts, uniformCounts));
//
//			backoffStrategies.addBackoffStrategy(new LimitedNaiveBackoffStrategy(po, cci, lm, &patternCounts, &contextCounts, uniformCounts, uniformLimitedCounts));
//		}
//	}
//
    std::cout << std::endl;


    NgramBackoffStrategy NBOS(po, cci, lm);



    Debug::getInstance() << DebugLevel::SUBPATTERN << "YOOOOOO PRINTING THIS SHIT\n";
//
//    if(qclo.backoffMethod.size())
//    {
//
		for(std::string inputFileName : po.testInputFiles)                          // files
//		std::vector<std::string> justSomeFiles;
//		justSomeFiles.push_back("/esat/spchtemp/scratch/jpeleman/lm/head_map/asr/comp-c/vl/fv701011/sri/fv701011_medthr256_kni4/nbest_fullvocab/fv701011.0.X:0:56004.50000-best.txt");
//    	for(std::string inputFileName : justSomeFiles)                          // files
		{
			std::cout << "> " << inputFileName << std::endl;
//
//			backoffStrategies.nextFile();
//			tsp.nextFile();
//
//	//        std::cout << "  Next file" << std::endl;
//
			std::ifstream file(inputFileName);
			std::string retrievedString;
			int currentRank = 0;
			NbestList nbl;
			while(std::getline(file, retrievedString))                              // lines
			{


				std::stringstream   linestream(retrievedString);

				std::string			acousticModelScoreString;
				double 				acousticModelScore;

				std::string			languageModelScoreString;
				double				languageModelScore;

				std::string			numberOfWordsString;
				int					numberOfWords;

				std::string 		sentenceString;

				std::getline(linestream, acousticModelScoreString, ' ');
				acousticModelScore = std::stod(acousticModelScoreString);

				std::getline(linestream, languageModelScoreString, ' ');
				languageModelScore = std::stod(languageModelScoreString);

				std::getline(linestream, numberOfWordsString, ' ');
				numberOfWords = std::stoi(numberOfWordsString);

				std::getline(linestream, sentenceString);

				NBestItem* nbi = new NBestItem(sentenceString, ++currentRank, acousticModelScore, languageModelScore, numberOfWords);
				nbl.add(nbi);

				std::vector<std::string> words = wsSplit(sentenceString);
				Debug::getInstance() << DebugLevel::ALL << "  Next line with " << words.size() << " words\n";
//
				double lprob = 0.0;
				int numberOfUsedPatterns = 0;

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

						try {
							Pattern context = cci.classEncoder.buildpattern(contextStream.str());
							Pattern focus = cci.classEncoder.buildpattern(words[i]);

							std::string top = "\n\tC[" + context.tostring(cci.classDecoder) + "] F[" + focus.tostring(cci.classDecoder) + "]\n";
							Debug::getInstance() << DebugLevel::PATTERN << top;

							std::string focusString = "";
							if(!allWords.has(focus)) // empty if oov
							{
								focusString = words[i];
							}

							lprob += NBOS.prob(focus, context, focusString);
							numberOfUsedPatterns++;

						} catch (const UnknownTokenError &e) {
						}
				   }
				}
				nbi->setRescore(lprob);
			}

			nbl.determineNewRanks();
			nbl.printToFile(inputFileName, qclo.outputDirectory);
		}

//		backoffStrategies.done();
//		std::cout << "\n\n" << std::endl;
//		backoffStrategies.printResults();
//    } else
//    {
//    	mout << "No backoff strategy was given." << std::endl;
//    }
//
//
//    delete mleLimitedCounts;
//    delete entropyLimitedCounts;
//    delete uniformLimitedCounts;
//    delete mleCounts;
//    delete entropyCounts;
//    delete uniformCounts;
}

