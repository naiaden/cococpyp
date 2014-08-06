#include <iostream>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"
#include "corpus/corpus.h"

#include "cpyp/boost_serializers.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <vector>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

/*
    1. Directory with test instances
    2. Directory from which we read all stuff, except for the input model
    3. Directory in which we put all stuff, except for the output model
    4. Whether we perform skipgrams
    5. Name of the input run
    6. Name of the output run
 */

int main(int argc, char** argv) {
    cpyp::MT19937 eng;

    std::string _test_input_directory = argv[1];
    std::string _input_directory = argv[2];
    std::string _output_directory = argv[3];
    bool _do_skipgrams = (std::atoi(argv[4]) != 0);
    std::string _input_run_name = argv[5];
    std::string _output_run_name = argv[6];

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

    PatternModelOptions _pattern_model_options = PatternModelOptions();
    _pattern_model_options.MAXLENGTH = kORDER;
    _pattern_model_options.MINLENGTH = 1; // kORDER - 1;
    _pattern_model_options.DOSKIPGRAMS = _do_skipgrams;
    _pattern_model_options.DOSKIPGRAMS_EXHAUSTIVE = _do_skipgrams;
    _pattern_model_options.DOREVERSEINDEX = true;
    _pattern_model_options.QUIET = false;
    _pattern_model_options.MINTOKENS = 1;

    boost::filesystem::path foreground_dir(_test_input_directory);
    boost::filesystem::directory_iterator fit(foreground_dir), feod;

    std::vector<std::string> test_input_files;
    BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod)) {
        if(is_regular_file(p)) {
            test_input_files.push_back(p.string());
        }
    }

    std::string _base_input_name = _input_directory + "/" + _input_run_name;
    std::string _input_class_file_name = _base_input_name + ".cls";
    std::string _input_corpus_file_name = _base_input_name + ".dat";
    std::string _input_patternmodel_file_name = _base_input_name + ".patternmodel";
    std::string _input_serialised_file_name = _base_input_name + ".ser";

    std::string _base_output_name = _output_directory + "/" + _output_run_name;
    std::string _output_class_file_name = _base_output_name + ".cls";
    std::string _output_corpus_file_name = _base_output_name + ".dat";
    std::string _output_patternmodel_file_name = _base_output_name + ".patternmodel";
    //std::string _output_serialised_file_name = _base_output_name + ".ser";
    std::string _output_probabilities_file_name = _base_output_name + ".probs";

    _class_encoder.load(_input_class_file_name);

    for(auto i : test_input_files) {
        _class_encoder.encodefile(i, _output_corpus_file_name, 1, 1, 0, 1);
    }
    _class_encoder.save(_output_class_file_name);

    _class_decoder.load(_output_class_file_name);

    IndexedCorpus _test_indexed_corpus = IndexedCorpus(_output_corpus_file_name);

    PatternModel<uint32_t> _test_pattern_model = PatternModel<uint32_t>(&_test_indexed_corpus);
    _test_pattern_model.train(_output_corpus_file_name, _pattern_model_options);

    _test_pattern_model.computestats();
    _test_pattern_model.computecoveragestats();

    _test_pattern_model.report(&std::cerr);


    std::cerr << ">" << _test_pattern_model.totalwordtypesingroup(0,1) << "<";

    

    _test_pattern_model.write(_output_patternmodel_file_name);

    double llh = 0;
    unsigned cnt = 0;
    unsigned oovs = 0;

    
    std::ifstream ifs(_input_serialised_file_name, std::ios::binary);
    if(!ifs.good()) {
        std::cerr << "Something went wrong whilst reading the model: " << _input_serialised_file_name << std::endl;
    }
    boost::archive::binary_iarchive ia(ifs);

    cpyp::PYPLM<kORDER> lm;
    ia & lm;

    std::ofstream _probs_file;
    _probs_file.open(_output_probabilities_file_name);

    for(IndexPattern indexPattern : _test_indexed_corpus) {
        for(Pattern pattern : _test_pattern_model.getreverseindex(indexPattern.ref)) {
            size_t pattern_size = pattern.size();

            Pattern context = Pattern();
            Pattern focus = Pattern();

            if(pattern_size == kORDER) {
                context = Pattern(pattern, 0, pattern_size - 1);
                focus = pattern[pattern_size - 1];

                if(pattern_size == 1) {
                    focus = pattern[0];
                } else {
                    context = Pattern(pattern, 0, pattern_size - 1);
                    focus = pattern[pattern_size - 1];
                }

                unsigned oc = _test_pattern_model.occurrencecount(focus);

                double lp = log(lm.prob(focus, context, &_class_decoder)) / log(2);
//                double lp = log(lm.glm_prob(focus, context, &_class_decoder)) / log(2);
//                double lp = log(lm.j7(focus, context, &_class_decoder)) / log(2);
//                double lp = log(lm.j7(focus, context)) / log(2);
                //std::cerr << " --- " << lp;
                if(!oc) {
                    ++oovs;
                    //std::cerr << " --- NO OC";
                    lp = 0;
                }
                //std::cerr << std::endl;

                _probs_file << "p[" << oc << "](" << focus.tostring(_class_decoder) << " |";
                _probs_file << context.tostring(_class_decoder) << ") = " << lp << std::endl;

                llh -= lp;
                ++cnt;
            }
        }
    }
    
    _probs_file.close();

    cnt -= oovs;
    std::cerr << "  Log_10 prob: " << (-llh * log(2) / log(10)) << std::endl;
    std::cerr << "        Count: " << cnt << std::endl;
    std::cerr << "         OOVs: " << oovs << std::endl;
    std::cerr << "Cross-entropy: " << (llh / cnt) << std::endl;
    std::cerr << "   Perplexity: " << pow(2, llh / cnt) << std::endl;
    return 0;

}

