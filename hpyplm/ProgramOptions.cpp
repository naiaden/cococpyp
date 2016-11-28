/*
 * ProgramOptions.cpp
 *
 *  Created on: Aug 31, 2016
 *      Author: louis
 */





#include "ProgramOptions.h"

#include "utils.h"
#include "Debug.h"

    CommandLineOptions::CommandLineOptions(int argc, char** argv)
    {
        initialise(argc, argv);
    }

    void CommandLineOptions::initialise(int argc, char** argv)
    {
        clp.add<std::string>("trainoutput", 'o', "train output directory", true);
        clp.add<std::string>("modelname", 'm', "the name of the training model", true);

        clp.add<std::string>("loadtraincorpus", '\0', "load colibri encoded corpus", false, "");
        clp.add<std::string>("loadtrainpatternmodel", '\0', "load colibri encoded pattern model", false, "");
        clp.add<std::string>("loadtrainvocabulary", '\0', "load colibri class file", false, "");
        clp.add<std::string>("loadtrainserialisedmodel", '\0', "load colibri serialised model", false, "");

        clp.add<std::string>("debug", '\0', "level of debug output", false, "NONE");
    }

    void CommandLineOptions::retrieve()
    {
        trainModelDirectory = clp.get<std::string>("trainoutput");
        trainModel = clp.get<std::string>("modelname");

        loadTrainCorpus = clp.get<std::string>("loadtraincorpus");
        loadTrainPatternModel = clp.get<std::string>("loadtrainpatternmodel");
        loadTrainVocabulary = clp.get<std::string>("loadtrainvocabulary");
        loadTrainSerialisedFile = clp.get<std::string>("loadtrainserialisedmodel");

        Debug::getInstance().set(clp.get<std::string>("debug"));
    }

    TrainCommandLineOptions::TrainCommandLineOptions(int argc, char** argv) : CommandLineOptions(argc, argv)
    {
        clp.add<std::string>("traininput", 'i', "train input directory", false);
        clp.add<std::string>("traininputfile", 'f', "train input file", false);

        clp.add<int>("samples", 's', "samples", false, 50);
        clp.add<int>("burnin", 'b', "burnin", false, 0);
        clp.add("skipgram", 'S', "train with skipgrams");

        clp.add<int>("sthreshold", 'T', "threshold for skipgrams", false, 1);
        clp.add<int>("threshold", 't', "threshold for ngrams", false, 1);
        clp.add<int>("unigramthreshold", 'W', "unigram threshold", false, 1);
        clp.add<int>("prunedonsubsumed", 'p', "prune all n-grams that are not subsumed by higher order n-grams", false, 0);

        clp.add<std::string>("extendmodel", 'E', "extend current model (with larger n or skips)", false, "");
        clp.parse_check(argc, argv);

        retrieve();
        trainModelInputDirectory = clp.get<std::string>("traininput");
        trainModelInput = clp.get<std::string>("traininputfile");

        samples = clp.get<int>("samples");
        burnin = clp.get<int>("burnin");
        skipgrams = clp.exist("skipgram");

        sThreshold = clp.get<int>("sthreshold");
        nThreshold = clp.get<int>("threshold");
        uThreshold = clp.get<int>("unigramthreshold");
        pruneLevel = clp.get<int>("prunedonsubsumed");

        extendModel = clp.get<std::string>("extendmodel");
    }

    AnalysisCommandLineOptions::AnalysisCommandLineOptions(int argc, char** argv) : CommandLineOptions(argc, argv)
    {
        clp.add<std::string>("analysisoutput", 'O', "analysis output directory", false, "");

        clp.parse_check(argc, argv);

        retrieve();

        outputDirectory = clp.get<std::string>("analysisoutput");
    }

    SNCBWCommandLineOptions::SNCBWCommandLineOptions(int argc, char** argv) : CommandLineOptions(argc, argv)
    {
        clp.add<int>("n", 'n', "n", false, 4);

        clp.add<std::string>("testmodel", 'M', "the name of the testing model", true);
        clp.add<std::string>("testinput", 'I', "test input directory", false);

        clp.add<std::string>("testinputfile", 'F', "test input file", false);
        clp.add<std::string>("testoutput", 'O', "test output directory", false, "");

        clp.add<std::string>("countfile", 'C', "count file base", false, "");

        clp.add<std::string>("limitedcachefile", 'L', "limited cache base file name", false, "");
        clp.add<std::string>("limiteduniformcachefile", 0, "limited uniform cache file with q values", false, "");
        clp.add<std::string>("limitedentropycachefile", 0, "limited entropy cache file with q values", false, "");
        clp.add<std::string>("limitedmlecachefile", 0, "limited mle cache file with q values", false, "");

//        clp.add<std::string>("backoff", 'B', "the backoff method", false, "ngram", cmdline::oneof<std::string>("glm", "bobaco", "ngram", "all"));
        clp.add<std::string>("backoff", 'B', "the backoff method", false, "ngram");
        clp.parse_check(argc, argv);

        retrieve();
        n = clp.get<int>("n");

        outputRunName = clp.get<std::string>("testmodel");
        testInputDirectory = clp.get<std::string>("testinput");
        testInputFile = clp.get<std::string>("testinputfile");
        testInputFiles = clp.rest();
        outputDirectory = clp.get<std::string>("testoutput");
        countFileBase = clp.get<std::string>("countfile");

        limitedCacheFile = clp.get<std::string>("limitedcachefile");
        limitedUniformCacheFile = clp.get<std::string>("limiteduniformcachefile");
        limitedEntropyCacheFile = clp.get<std::string>("limitedentropycachefile");
        limitedMLECacheFile = clp.get<std::string>("limitedmlecachefile");

        backoffMethod = fromString(clp.get<std::string>("backoff"));
    }

    QueryCommandLineOptions::QueryCommandLineOptions(int argc, char** argv) : CommandLineOptions(argc, argv)
    {
        clp.add<std::string>("testinput", 'I', "test input directory", false);
       testInputDirectory = clp.get<std::string>("testinput");

        clp.add<std::string>("testinputfile", 'F', "test input file", false);
       testInputFile = clp.get<std::string>("testinputfile");

        clp.add<std::string>("sharedoutput", '\0', "train and test output directory", false, "");
        sharedOutputDirectory = clp.get<std::string>("sharedoutput");

        clp.add<std::string>("testoutput", 'O', "test output directory", false, "");
       outputDirectory = clp.get<std::string>("testoutput");

        clp.add<std::string>("testmodel", 'M', "the name of the testing model", true);
        outputRunName = clp.get<std::string>("testmodel");

        clp.add<std::string>("backoff", 'B', "the backoff method", false, "ngram", cmdline::oneof<std::string>("glm", "bobaco", "ngram", "all"));
        backoffMethod = fromString(clp.get<std::string>("backoff"));


        clp.parse_check(argc, argv);


        if(clp.get<std::string>("output").empty()
                && (clp.get<std::string>("testoutput").empty()
           || clp.get<std::string>("trainoutput").empty()))
        {
            std::cerr << "Provide either an output, or train and test output directories." << std::endl;
        } else if(!clp.get<std::string>("output").empty()
                && !clp.get<std::string>("testoutput").empty()
                && !clp.get<std::string>("trainoutput").empty())
        {
            std::cerr << "Ignoring output, but using testoutput and trainoutput." << std::endl;
        } else if(!clp.get<std::string>("output").empty())
        {
            trainModelDirectory = clp.get<std::string>("output");
            outputDirectory = clp.get<std::string>("output");
        }


    }


    ProgramOptions::ProgramOptions(CommandLineOptions& _clo) : clo(_clo)
    {
        char hostname[128];
        gethostname(hostname, sizeof hostname);
        hostName = std::string(hostname);

       trainRunName = clo.trainModelDirectory + "/" + clo.trainModel;
       std::cout << "PO: trainRunName = " << trainRunName << std::endl;

        if(clo.loadTrainVocabulary.empty())
        {
            trainClassFileName = trainRunName + ".cls";
        } else
        {
            trainClassFileName = clo.loadTrainVocabulary;
        }
        std::cout << "PO: trainClassFileName = " << trainClassFileName << std::endl;

        if(clo.loadTrainCorpus.empty())
        {
       trainCorpusFileName = trainRunName + ".dat";
       } else
       {
        trainCorpusFileName = clo.loadTrainCorpus;
        }
        std::cout << "PO: trainCorpusFileName = " << trainCorpusFileName << std::endl;

        if(clo.loadTrainPatternModel.empty())
        {
       trainPatternModelFileName = trainRunName + ".patternmodel";
       } else
       {
       trainPatternModelFileName = clo.loadTrainPatternModel;
       }
       std::cout << "PO: trainPatternModelFileName = " << trainPatternModelFileName << std::endl;

        if(clo.loadTrainSerialisedFile.empty())
        {
       trainSerialisedFileName = trainRunName + ".ser";
       } else
       {
        trainSerialisedFileName = clo.loadTrainSerialisedFile;
       }
       std::cout << "PO: trainSerialisedFileName = " << trainSerialisedFileName << std::endl;
    }

    AnalysisProgramOptions::AnalysisProgramOptions(AnalysisCommandLineOptions& _clo) : ProgramOptions(_clo)
    {

        baseOutputName = _clo.outputDirectory + "/" + _clo.trainModel + "-analysis";
        generalBaseOutputName = baseOutputName + ".res";

    }

    SNCBWProgramOptions::SNCBWProgramOptions(SNCBWCommandLineOptions& _clo, int _n) : ProgramOptions(_clo)
    {
        n = _n;
        testInputFiles = _clo.testInputFiles;
        if(!_clo.testInputDirectory.empty())
        {
            boost::filesystem::path foreground_dir(_clo.testInputDirectory);
            boost::filesystem::directory_iterator fit(foreground_dir), feod;
            BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod))
            {
                if(is_regular_file(p))
                {
                    testInputFiles.push_back(p.string());
                }
            }
        } else
        {
//            if(_clo.testInputFile.empty())
//            {
//                std::cerr << "Did you use -f instead of -F? No input files for testing found." << std::endl;
//            }
        	if(!_clo.testInputFile.empty())
        	{
        		testInputFiles.push_back(_clo.testInputFile);
        	}
        }
        for(auto f: testInputFiles)
        {
            std::cout << f << std::endl;
        }

        baseOutputName = _clo.outputDirectory + "/" + _clo.outputRunName;
        generalBaseOutputName = baseOutputName + "_" + backoffsToString(_clo.backoffMethod)
                                               + "-common_" + std::to_string(_n);
        generalOutputClassFileName = generalBaseOutputName + ".cls";
        generalOutputCorpusFileName = generalBaseOutputName + ".dat";

        if(clo.loadTrainSerialisedFile.empty())
		{
        	generalLimitedCacheFileName = generalBaseOutputName + ".lim";
		} else
		{
			generalLimitedCacheFileName = _clo.limitedCacheFile;
		}
	   std::cout << "PO: generalLimitedCacheFileName = " << generalLimitedCacheFileName << std::endl;

	   limitedEntropyCacheFile = _clo.limitedEntropyCacheFile;
	   limitedMLECacheFile = _clo.limitedMLECacheFile;
	   limitedUniformCacheFile = _clo.limitedUniformCacheFile;

        countFilesBase = _clo.countFileBase;
    }

    TrainProgramOptions::TrainProgramOptions(TrainCommandLineOptions& _clo, int _n) : ProgramOptions(_clo)
    {
        n = _n;

        if(!_clo.trainModelInputDirectory.empty()) {
            boost::filesystem::path background_dir(_clo.trainModelDirectory);
            boost::filesystem::directory_iterator bit(background_dir), beod;

            BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, beod)) {
                if(is_regular_file(p)) {
                     trainInputFiles.push_back(p.string());
                }
            }
        } else {
            trainInputFiles.push_back(_clo.trainModelInput);
        }

        generalBaseOutputName = trainRunName + "_" + std::to_string(n)
                                                  + (_clo.skipgrams ? "S" : "")
                                                  + "_W" + std::to_string(_clo.uThreshold)
                                                  + "_t" + std::to_string(_clo.nThreshold)
                                                  + "_T" + std::to_string(_clo.sThreshold)
                                                  + "_s" + std::to_string(_clo.samples)
                                                  + "_p" + std::to_string(_clo.pruneLevel)
                                                  + "_v2"
                                                  + "_train";
        std::cout << "TPO: generalBaseOutputName = " << generalBaseOutputName << std::endl;
        generalBaseClassFileName = generalBaseOutputName + ".cls";
        generalBaseCorpusFileName = generalBaseOutputName + ".dat";
        generalBasePatternModelFileName = generalBaseOutputName + ".patternmodel";
        generalBaseSerialisedFileName = generalBaseOutputName + ".ser";

        extendModel = _clo.extendModel;
        samples = _clo.samples;
    }


    QueryProgramOptions::QueryProgramOptions(QueryCommandLineOptions& _clo, int _n) : ProgramOptions(_clo)
    {
        n = _n;
         if(!_clo.testInputDirectory.empty()) {
             boost::filesystem::path foreground_dir(_clo.testInputDirectory);
             boost::filesystem::directory_iterator fit(foreground_dir), feod;

             BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod)) {
                 if(is_regular_file(p)) {
                     testInputFiles.push_back(p.string());
                 }
             }
         } else {
             testInputFiles.push_back(_clo.testInputFile);
         }
         for(auto f: testInputFiles)
         {
             std::cout << f << std::endl;
         }


         generalBaseOutputName = _clo.outputDirectory + "/" + _clo.outputRunName + "_" + backoffsToString(_clo.backoffMethod) + "-common_" + std::to_string(_n);
         generalOutputClassFileName = generalBaseOutputName + ".cls";
         generalOutputCorpusFileName = generalBaseOutputName + ".dat";



    }

