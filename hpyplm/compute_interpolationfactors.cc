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

int main(int argc, char** argv) 
{
    cmdline::parser clp;

    clp.add<std::string>("trainoutput", 'O', "train output directory", true);
    clp.add<std::string>("output", 'o', "output directory", true);
    clp.add<std::string>("trainmodel", 'm', "the name of the training model", true);
    clp.add<int>("n", 'n', "n", false, 4);

    clp.parse_check(argc, argv);

    std::string _input_directory = clp.get<std::string>("trainoutput");
    std::string _output_directory = clp.get<std::string>("output");
    std::string _input_run_name = clp.get<std::string>("trainmodel");

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

    PatternModelOptions _pattern_model_options = PatternModelOptions();
    _pattern_model_options.MAXLENGTH = 4;
    _pattern_model_options.MINLENGTH = 1;
    _pattern_model_options.DOSKIPGRAMS = false;
    _pattern_model_options.DOREVERSEINDEX = true;
    _pattern_model_options.QUIET = false;
    _pattern_model_options.MINTOKENS = 1;

   std::string _base_input_name = _input_directory + "/" + _input_run_name;
   std::string _input_class_file_name = _base_input_name + ".cls";
   std::string _input_corpus_file_name = _base_input_name + ".dat";
   std::string _input_patternmodel_file_name = _base_input_name + ".patternmodel";

   std::string _general_base_output_name = _output_directory + "/" + _input_run_name;
   std::string _general_interpolationfactors_output_name = _general_base_output_name + ".factors";

   std::ofstream _general_output;
   _general_output.open(_general_interpolationfactors_output_name);

   _class_encoder.load(_input_class_file_name);
   std::cout << "Done loading class encoder" << std::endl;
   _class_decoder.load(_input_class_file_name);
   std::cout << "Done loading class decoder" << std::endl;

   IndexedCorpus _indexed_corpus = IndexedCorpus(_input_corpus_file_name);
   std::cout << "Done loading indexed corpus" << std::endl;
   
   PatternModel<uint32_t> _train_pattern_model(_input_patternmodel_file_name, _pattern_model_options, nullptr, &_indexed_corpus);
   PatternSet<uint64_t> allWords = _train_pattern_model.extractset(1,1);
    std::cout << "Done creating pattern set" << std::endl;

    int bla = 0;
    for(int n = 3; n <= 4; ++n)
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
    
                std::cout << prefix.tostring(_class_decoder) << "\t" << added_patterns.size() << "\t" << -llh << "\t" << llh/added_patterns.size() << std::endl;
                std::cout << "----------------------------------------" << std::endl;
                previous_prefix = prefix;

                llh = 0;
                sum = 0;
                added_patterns = std::vector<int>();
            }
            std::cout << "[" << prefix.tostring(_class_decoder) << "]\t" << pattern.tostring(_class_decoder) << std::endl;
            int count = _train_pattern_model.occurrencecount(pattern);
            sum += count;
            added_patterns.push_back(count);
        }
    }

   _general_output.close();
}
