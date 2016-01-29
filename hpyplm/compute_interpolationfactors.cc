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
   PatternSet<uint64_t> allPatterns = _train_pattern_model.extractset(4,4);
   PatternSet<uint64_t> allWords = _train_pattern_model.extractset(1,1);
    std::cout << "Done creating pattern set" << std::endl;

    PatternSet<uint64_t> set_of_all_contexts = _train_pattern_model.extractset(1,1);//allWords;
    PatternSet<uint64_t> set_of_contexts = PatternSet<uint64_t>();

    int bla = 0;
    for(int n = 1; n < 4; ++n)
    {
        bla = 0;
        std::cout << "Generating contexts with length " << n << std::endl;
        for(auto context : set_of_all_contexts)
        {
            int sum = 0;
            int triggers = 0;
            std::vector<int> occs;
            for(auto word : allWords)
            {
                const Pattern ngram = context + word;
                int count = _train_pattern_model.occurrencecount(ngram);
                if(count)
                {
                    occs.push_back(count);
                    ++triggers;
                    sum += count;
                    set_of_contexts.insert(ngram);
                    std::cout << "Adding " << context.tostring(_class_decoder) << std::endl;
                    // possible optimisation: only insert context once
                }
            }
            
            double llh = 0;
            for(auto occ : occs)
            {
                double mle = occ*1.0/sum;
                llh -= log(mle);
            }

            //std::cout << context.tostring(_class_decoder) << "\t" << triggers << "\t" << -llh << "\t" << llh/triggers << std::endl;
            if(++bla == 100)
            {
                break;
            }
        }

        set_of_all_contexts = set_of_contexts;
        set_of_contexts = PatternSet<uint64_t>();
        // dit gaat fout...
    }

   _general_output.close();
}
