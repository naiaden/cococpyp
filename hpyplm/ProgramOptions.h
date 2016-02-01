#ifndef PROGRAMOPTIONS_H_
#define PROGRAMOPTIONS_H_

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
    std::string trainModelDirectory;
    std::string loadTrainVocabulary;
    std::string loadTrainCorpus;
    std::string loadTrainPatternModel;
    std::string loadTrainSerialisedFile;

};

struct SNCBWCommandLineOptions : public CommandLineOptions
{
    std::string outputDirectory;
    int n;

    SNCBWCommandLineOptions(int argc, char** argv)
    {
        clp.add<std::string>("trainoutput", 'O', "train output directory", true);
        clp.add<std::string>("output", 'o', "output directory", true);
        clp.add<std::string>("trainmodel", 'm', "the name of the training model", true);
        clp.add<int>("n", 'n', "n", false, 4);

        clp.parse_check(argc, argv);

        outputDirectory = clp.get<std::string>("output");
        trainModelDirectory = clp.get<std::string>("trainoutput");
        trainModel = clp.get<std::string>("trainmodel");
        n = clp.get<int>("n");

    }
};

struct QueryCommandLineOptions : public CommandLineOptions
{
    std::string testInputDirectory;
    std::string testInputFile;

    std::string outputDirectory;
    std::string outputRunName;
    Backoff backoffMethod;
    
    QueryCommandLineOptions(int argc, char** argv)
    {
        clp.add<std::string>("testinput", 'I', "test input directory", false);
        clp.add<std::string>("testinputfile", 'F', "test input file", false); 
        clp.add<std::string>("output", '\0', "train and test output directory", false, "");
        clp.add<std::string>("testoutput", 'o', "test output directory", false, "");
        clp.add<std::string>("trainoutput", 'O', "train output directory", false, "");

        clp.add<std::string>("trainmodel", 'm', "the name of the training model", true);
        clp.add<std::string>("testmodel", 'M', "the name of the testing model", true);

        clp.add<std::string>("backoff", 'B', "the backoff method", false, "ngram", cmdline::oneof<std::string>("glm", "bobaco", "ngram", "all"));

        clp.add<std::string>("loadcorpus", '\0', "load colibri encoded corpus", false, "");
        clp.add<std::string>("loadpatternmodel", '\0', "load colibri encoded pattern model", false, "");
        clp.add<std::string>("loadvocabulary", '\0', "load colibri class file", false, "");

        clp.parse_check(argc, argv);

       testInputDirectory = clp.get<std::string>("testinput");
       testInputFile = clp.get<std::string>("testinputfile");
       trainModelDirectory = clp.get<std::string>("trainoutput");
       outputDirectory = clp.get<std::string>("testoutput");
        
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

        trainModel = clp.get<std::string>("trainmodel");
        outputRunName = clp.get<std::string>("testmodel");
        backoffMethod = fromString(clp.get<std::string>("backoff"));

        loadTrainCorpus = clp.get<std::string>("loadcorpus");
        loadTrainPatternModel = clp.get<std::string>("loadpatternmodel");
        loadTrainVocabulary = clp.get<std::string>("loadvocabulary");

    }

};

struct ProgramOptions
{
    CommandLineOptions& clo;

   std::string trainRunName;
   std::string trainClassFileName; 
   std::string trainCorpusFileName;
   std::string trainPatternModelFileName; 
   std::string trainSerialisedFileName;

    ProgramOptions(CommandLineOptions& _clo) : clo(_clo)
    {
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

   std::string generalBaseOutputName; 
   std::string generalInterpolationFactorsOutputName; 

    SNCBWProgramOptions(SNCBWCommandLineOptions& _clo) : ProgramOptions(_clo)
    {

       generalBaseOutputName = _clo.outputDirectory + "/" + clo.trainModel; 
       generalInterpolationFactorsOutputName = generalBaseOutputName + ".factors"; 

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
