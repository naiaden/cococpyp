#ifndef PROGRAMOPTIONS_H_
#define PROGRAMOPTIONS_H_

#include <cstdlib>

#include "cmdline.h"

enum class Backoff { GLM, BOBACO, NGRAM, REL, ALL};

Backoff fromString(const std::string& s) {
    if(s.compare("glm") == 0) return Backoff::GLM;
    else if(s.compare("bobaco") == 0) return Backoff::BOBACO;
    else if(s.compare("relative") == 0) return Backoff::REL;
    else if(s.compare("all") == 0) return Backoff::ALL;
    else return Backoff::NGRAM;
}

std::string toString(Backoff b) {
    if(b == Backoff::GLM) return "glm";
    if(b == Backoff::BOBACO) return "bobaco";
    if(b == Backoff::NGRAM) return "ngram";
    if(b == Backoff::REL) return "relative";
    if(b == Backoff::ALL) return "all";
    return "unknown backoff method";
}


struct CommandLineOptions
{
    cmdline::parser clp;

    std::string trainModel;
    std::string trainModelInput;
    std::string trainModelDirectory;
    std::string loadTrainVocabulary;
    std::string loadTrainCorpus;
    std::string loadTrainPatternModel;
    std::string loadTrainSerialisedFile;

    CommandLineOptions(int argc, char** argv)
    {
        clp.add<std::string>("traininput", 'i', "train input directory", false);
        trainModelDirectory = clp.get<std::string>("traininput");

        clp.add<std::string>("traininputfile", 'f', "train input file", false);
        trainModelInput = clp.get<std::string>("traininputfile");

        clp.add<std::string>("trainoutput", 'o', "train output directory", true);
        trainModelDirectory = clp.get<std::string>("trainoutput");

        clp.add<std::string>("modelname", 'm', "the name of the training model", true);
        trainModel = clp.get<std::string>("modelname");


        clp.add<std::string>("loadtraincorpus", '\0', "load colibri encoded corpus", false, "");
        loadTrainCorpus = clp.get<std::string>("loadtraincorpus");

        clp.add<std::string>("loadtrainpatternmodel", '\0', "load colibri encoded pattern model", false, "");
        loadTrainPatternModel = clp.get<std::string>("loadtrainpatternmodel");

        clp.add<std::string>("loadtrainvocabulary", '\0', "load colibri class file", false, "");
        loadTrainVocabulary = clp.get<std::string>("loadtrainvocabulary");

        clp.add<std::string>("loadtrainserialisedmodel", '\0', "load colibri serialised model", false, "");
        loadTrainSerialisedFile = clp.get<std::string>("loadtrainserialisedmodel");
    }
};

struct TrainCommandLineOptions : public CommandLineOptions
{
    int samples;
    int burnin;

    bool skipgrams;

    int sThreshold;
    int nThreshold;
    int uThreshold;
    int pruneLevel;

    std::string extendModel;

    TrainCommandLineOptions(int argc, char** argv) : CommandLineOptions(argc, argv)
    {
        clp.add<int>("samples", 's', "samples", false, 50);
        samples = clp.get<int>("samples");

        clp.add<int>("burnin", 'b', "burnin", false, 0);
        burnin = clp.get<int>("burnin");

        clp.add("skipgram", 'S', "train with skipgrams");
        skipgrams = clp.exist("skipgram");


        clp.add<int>("sthreshold", 'T', "threshold for skipgrams", false, 1);
        sThreshold = clp.get<int>("sthreshold");

        clp.add<int>("threshold", 't', "threshold for ngrams", false, 1);
        nThreshold = clp.get<int>("threshold");

        clp.add<int>("unigramthreshold", 'W', "unigram threshold", false, 1);
        uThreshold = clp.get<int>("unigramthreshold");

        clp.add<int>("prunedonsubsumed", 'p', "prune all n-grams that are not subsumed by higher order n-grams", false, 0);
        pruneLevel = clp.get<int>("prunedonsubsumed");

        clp.add<std::string>("extendmodel", 'E', "extend current model (with larger n or skips)", false, "");
        extendModel = clp.get<std::string>("extendmodel");

        clp.parse_check(argc, argv);
    }

};

struct SNCBWCommandLineOptions : public CommandLineOptions
{
    std::string outputDirectory;
    int n;

    SNCBWCommandLineOptions(int argc, char** argv) : CommandLineOptions(argc, argv)
    {
        clp.add<std::string>("output", 'O', "output directory", true);
        outputDirectory = clp.get<std::string>("output");

        clp.add<int>("n", 'n', "n", false, 4);
        n = clp.get<int>("n");

        clp.parse_check(argc, argv);
    }
};

struct QueryCommandLineOptions : public CommandLineOptions
{
    std::string testInputDirectory;
    std::string testInputFile;

    std::string sharedOutputDirectory;
    std::string outputDirectory;
    std::string outputRunName;
    Backoff backoffMethod;
    
    QueryCommandLineOptions(int argc, char** argv) : CommandLineOptions(argc, argv)
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

};

struct ProgramOptions
{
    CommandLineOptions& clo;

    std::string hostName;

   std::string trainRunName;
   std::string trainClassFileName; 
   std::string trainCorpusFileName;
   std::string trainPatternModelFileName; 
   std::string trainSerialisedFileName;

    ProgramOptions(CommandLineOptions& _clo) : clo(_clo)
    {
        char hostname[128];
        gethostname(hostname, sizeof hostname);
        std::string hostName(hostname);

       trainRunName = clo.trainModelDirectory + "/" + clo.trainModel;
       
        if(clo.loadTrainVocabulary.empty())
        {
            trainClassFileName = trainRunName + ".cls"; 
        } else
        {
            trainClassFileName = clo.loadTrainVocabulary;
        }

        if(clo.loadTrainCorpus.empty())
        {
       trainCorpusFileName = trainRunName + ".dat";
       } else
       {
        trainCorpusFileName = clo.loadTrainCorpus;
        }

        if(clo.loadTrainPatternModel.empty())
        {
       trainPatternModelFileName = trainRunName + ".patternmodel"; 
       } else
       {
       trainPatternModelFileName = clo.loadTrainCorpus;
       }

        if(clo.loadTrainSerialisedFile.empty())
        {
       trainSerialisedFileName = trainRunName + ".ser"; 
       } else
       {
        trainSerialisedFileName = clo.loadTrainSerialisedFile;
       }
    }
};

struct SNCBWProgramOptions : public ProgramOptions
{
    int n;

   std::string generalBaseOutputName; 
   std::string generalInterpolationFactorsOutputName; 

    SNCBWProgramOptions(SNCBWCommandLineOptions& _clo, int _n) : ProgramOptions(_clo)
    {
        n = _n;

       generalBaseOutputName = _clo.outputDirectory + "/" + clo.trainModel; 
       generalInterpolationFactorsOutputName = generalBaseOutputName + ".factors"; 

    }
};

struct TrainProgramOptions : public ProgramOptions
{
//    std::string testRunName;
    int n;

    std::vector<std::string> trainInputFiles;
    std::string generalBaseOutputName;
    std::string generalBaseClassFileName;
    std::string generalBaseCorpusFileName;
    std::string generalBasePatternModelFileName;
    std::string generalBaseSerialisedFileName;

    TrainProgramOptions(TrainCommandLineOptions& _clo, int _n) : ProgramOptions(_clo)
    {
        n = _n;

        if(!clo.trainModelDirectory.empty()) {
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
        generalBaseClassFileName = generalBaseOutputName + ".cls";
        generalBaseCorpusFileName = generalBaseOutputName + ".dat";
        generalBasePatternModelFileName = generalBaseOutputName + ".patternmodel";
        generalBaseSerialisedFileName = generalBaseOutputName + ".ser";

    }
};

struct QueryProgramOptions : public ProgramOptions
{
    std::string testRunName;
    int n;

    std::string generalBaseOutputName;
    std::string generalOutputClassFileName;
    std::string generalOutputCorpusFileName;
    
    std::vector<std::string> testInputFiles;
    QueryProgramOptions(QueryCommandLineOptions& _clo, int _n) : ProgramOptions(_clo)
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

        
         generalBaseOutputName = _clo.outputDirectory + "/" + _clo.outputRunName + "_" + toString(_clo.backoffMethod) + "-common_" + std::to_string(_n);
         generalOutputClassFileName = generalBaseOutputName + ".cls";
         generalOutputCorpusFileName = generalBaseOutputName + ".dat";
    


    }


};


#endif
/*

     std::ofstream _general_output;
     std::string _general_output_filename = _general_base_output_name + ".output";
     _general_output.open(_general_output_filename);

*/
