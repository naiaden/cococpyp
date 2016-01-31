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

struct PatternComp
{
    bool operator() (const Pattern& lhs, const Pattern& rhs) const
    {
        return lhs>rhs;
    }
};

struct CommandLineOptions
{
    cmdline::parser clp;
    std::string _output_directory;
    std::string _input_directory;
    std::string _train_model;
    int n;

    CommandLineOptions(int argc, char** argv)
    {
        clp.add<std::string>("trainoutput", 'O', "train output directory", true);
        clp.add<std::string>("output", 'o', "output directory", true);
        clp.add<std::string>("trainmodel", 'm', "the name of the training model", true);
        clp.add<int>("n", 'n', "n", false, 4);

        clp.parse_check(argc, argv);

        _output_directory = clp.get<std::string>("output");
        _input_directory = clp.get<std::string>("trainoutput");
        _train_model = clp.get<std::string>("trainmodel");
        n = clp.get<int>("n");

    }

   std::string getInputDirectory() { return _input_directory; }//std::string ret = clp.get<std::string>("trainoutput"); return ret;}
   std::string getOutputDirectory() { return _output_directory; } 
   std::string getTrainModel() { return _train_model; } //return clp.get<std::string>("trainmodel"); }
   int getN() { return n; }

};

struct CustomPatternModelOptions
{
    PatternModelOptions _pattern_model_options = PatternModelOptions();
  
  CustomPatternModelOptions()
   {
        _pattern_model_options.MAXLENGTH = 4;
        _pattern_model_options.MINLENGTH = 1;
        _pattern_model_options.DOSKIPGRAMS = false;
        _pattern_model_options.DOREVERSEINDEX = true;
        _pattern_model_options.QUIET = false;
        _pattern_model_options.MINTOKENS = 1;
   }

    PatternModelOptions getPatternModelOptions() const { return _pattern_model_options; } 
};

struct ProgramOptions
{
    CommandLineOptions& clo;

    ProgramOptions(CommandLineOptions& _clo) : clo(_clo)
    {
        
        
    }


   std::string getBaseInputName() { return clo.getInputDirectory() + "/" + clo.getTrainModel(); }
   std::string getInputClassFileName() { return getBaseInputName() + ".cls"; }
   std::string getInputCorpusFileName() { return getBaseInputName() + ".dat"; }
   std::string getInputPatternModelFileName() { return getBaseInputName() + ".patternmodel"; }

   std::string getGeneralBaseOutputName() { return clo.getOutputDirectory() + "/" + clo.getTrainModel(); }
   std::string getGeneralInterpolationFactorsOutputName() { return getGeneralBaseOutputName() + ".factors"; }
};

int main(int argc, char** argv) 
{
    CommandLineOptions clo = CommandLineOptions(argc, argv);
    ProgramOptions po = ProgramOptions(clo);
    CustomPatternModelOptions cpmo = CustomPatternModelOptions();

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

   std::ofstream _general_output;
   _general_output.open(po.getGeneralInterpolationFactorsOutputName());

    std::cout << "Processing " << po.getInputClassFileName() << std::endl;

   _class_encoder.load(po.getInputClassFileName());
   std::cout << "Done loading class encoder" << std::endl;
   _class_decoder.load(po.getInputClassFileName());
   std::cout << "Done loading class decoder" << std::endl;

   IndexedCorpus _indexed_corpus = IndexedCorpus(po.getInputCorpusFileName());
   std::cout << "Done loading indexed corpus" << std::endl;
   
   PatternModel<uint32_t> _train_pattern_model(po.getInputPatternModelFileName(), cpmo.getPatternModelOptions(), nullptr, &_indexed_corpus);

    for(int n = 1; n <= 4; ++n)
    {
        PatternSet<uint64_t> allPatterns = _train_pattern_model.extractset(n,n);
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
    
                std::cout << previous_prefix.tostring(_class_decoder) << "\t" << added_patterns.size() << "\t" << -llh << "\t" << llh/added_patterns.size() << std::endl;
//                _general_output << previous_prefix.tostring(_class_decoder) << "\t" << added_patterns.size() << "\t" << -llh << "\t" << llh/added_patterns.size() << std::endl;
                previous_prefix = prefix;

                llh = 0;
                sum = 0;
                added_patterns = std::vector<int>();
            }
            
            int count = _train_pattern_model.occurrencecount(pattern);
            sum += count;
            added_patterns.push_back(count);
        }
    }

   _general_output.close();
}
