#ifndef PROGRAMOPTIONS_H_
#define PROGRAMOPTIONS_H_

#include <cstdlib>

#include "cmdline.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "utils.h"


struct CommandLineOptions
{
    cmdline::parser clp;

    std::string trainModel;
    std::string trainModelDirectory;
    std::string loadTrainVocabulary;
    std::string loadTrainCorpus;
    std::string loadTrainPatternModel;
    std::string loadTrainSerialisedFile;

    CommandLineOptions(int argc, char** argv);
    
    void initialise(int argc, char** argv);

    void retrieve();
};

struct TrainCommandLineOptions : public CommandLineOptions
{
    std::string trainModelInput;
    std::string trainModelInputDirectory;

    int samples;
    int burnin;

    bool skipgrams;

    int sThreshold;
    int nThreshold;
    int uThreshold;
    int pruneLevel;

    std::string extendModel;

    TrainCommandLineOptions(int argc, char** argv);

};

struct AnalysisCommandLineOptions : public CommandLineOptions
{
    std::string outputDirectory;

    AnalysisCommandLineOptions(int argc, char** argv);
};

struct SNCBWCommandLineOptions : public CommandLineOptions
{
    std::string testInputDirectory;
    std::string testInputFile;

    std::string outputDirectory;
    std::string outputRunName;

    std::string limitedCacheFile;
    std::string limitedEntropyCacheFile;
    std::string limitedMLECacheFile;
    std::string limitedUniformCacheFile;

    std::string countFileBase;

    int n;

    Backoff backoffMethod;

    SNCBWCommandLineOptions(int argc, char** argv);
};

struct QueryCommandLineOptions : public CommandLineOptions
{
    std::string testInputDirectory;
    std::string testInputFile;

    std::string sharedOutputDirectory;
    std::string outputDirectory;
    std::string outputRunName;
    Backoff backoffMethod;
    
    QueryCommandLineOptions(int argc, char** argv);

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

    virtual ~ProgramOptions() {}

    ProgramOptions(CommandLineOptions& _clo);
};

struct AnalysisProgramOptions : public ProgramOptions
{

    std::string baseOutputName;
    std::string generalBaseOutputName;


    std::string countFilesBase;

    AnalysisProgramOptions(AnalysisCommandLineOptions& _clo);
};

struct SNCBWProgramOptions : public ProgramOptions
{
    std::string testRunName;
    int n;
    std::string baseOutputName;
    std::string generalBaseOutputName;
    std::string generalOutputClassFileName;
    std::string generalOutputCorpusFileName;
    std::string generalLimitedCacheFileName;
    std::vector<std::string> testInputFiles;
    
    std::string countFilesBase;

    std::string limitedEntropyCacheFile;
    std::string limitedMLECacheFile;
    std::string limitedUniformCacheFile;

    SNCBWProgramOptions(SNCBWCommandLineOptions& _clo, int _n);
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

    std::string extendModel;
    int samples;

    TrainProgramOptions(TrainCommandLineOptions& _clo, int _n);
};

struct QueryProgramOptions : public ProgramOptions
{
    std::string testRunName;
    int n;

    std::string generalBaseOutputName;
    std::string generalOutputClassFileName;
    std::string generalOutputCorpusFileName;
    
    std::vector<std::string> testInputFiles;
    QueryProgramOptions(QueryCommandLineOptions& _clo, int _n);


};


#endif
