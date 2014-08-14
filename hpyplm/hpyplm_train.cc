#include <iostream>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"
#include "corpus/corpus.h"
#include "cpyp/m.h"
#include "cpyp/random.h"
#include "cpyp/crp.h"
#include "cpyp/tied_parameter_resampler.h"

#include "cpyp/boost_serializers.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <vector>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include "hpyplm/uniform_vocab.h"
#include "cmdline.h"

int main(int argc, char** argv) {
    cpyp::MT19937 _eng;
   
    cmdline::parser clp;

    clp.add<std::string>("traininput", 'i', "train input directory", true);
    clp.add<std::string>("trainoutput", 'o', "train output directory", true);
    
    clp.add<int>("samples", 's', "samples", false, 50);
    clp.add<int>("burnin", 'b', "burnin", false, 0);

    clp.add("skipgram", 'S', "train with skipgrams");
    clp.add<int>("streshold", 'T', "treshold for skipgrams", false, 1);
    clp.add<int>("treshold", 't', "treshold for ngrams", false, 1);

    clp.add<std::string>("modelname", 'm', "the name of the training model", true);

    clp.parse_check(argc, argv);

    std::string _train_input_directory = clp.get<std::string>("traininput");
    std::string _output_directory = clp.get<std::string>("trainoutput");

    int _samples = clp.get<int>("samples");
    int _burnin = clp.get<int>("burnin");

    bool _do_skipgrams = clp.exist("skipgram");
    int _min_skip_tokens = clp.get<int>("streshold");
    int _min_tokens = clp.get<int>("treshold");

    std::string _run_name = clp.get<std::string>("modelname");

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

    if(_do_skipgrams) { std::cerr << "Generating skipgrams, fwiw..." << std::endl; }

    PatternModelOptions _pattern_model_options = PatternModelOptions();
    _pattern_model_options.MAXLENGTH = kORDER;
    _pattern_model_options.MINLENGTH = 1; // kORDER - 1
    _pattern_model_options.DOSKIPGRAMS = _do_skipgrams;
    _pattern_model_options.DOSKIPGRAMS_EXHAUSTIVE = _do_skipgrams;
    _pattern_model_options.DOREVERSEINDEX = true;
    _pattern_model_options.QUIET = false;
    _pattern_model_options.MINTOKENS = _min_tokens;
    _pattern_model_options.MINTOKENS_SKIPGRAMS = _min_skip_tokens;

    std::cerr << "Min tokens: " << _pattern_model_options.MINTOKENS;
    std::cerr << ", min skip tokens: " << _pattern_model_options.MINTOKENS_SKIPGRAMS << std::endl;

    boost::filesystem::path background_dir(_train_input_directory);
    //boost::filesystem::recursive_directory_iterator bit(background_dir), beod;
    boost::filesystem::directory_iterator bit(background_dir), beod;

    std::vector<std::string> train_input_files;
    BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, beod)) {
        if(is_regular_file(p)) {
            train_input_files.push_back(p.string());
        }
    }

    std::string _base_name = _output_directory + "/" + _run_name;
    std::string _class_file_name = _base_name + ".cls";
    std::string _corpus_file_name = _base_name + ".dat";
    std::string _patternmodel_file_name = _base_name + ".patternmodel";
    std::string _serialised_file_name = _base_name + ".ser";

    _class_encoder.build(train_input_files, true);
    _class_encoder.save(_class_file_name);

    for(auto i:train_input_files) {
        _class_encoder.encodefile(i, _corpus_file_name, 0, 0, 1, 0);
    }
    _class_decoder.load(_class_file_name);

    IndexedCorpus _indexed_corpus = IndexedCorpus(_corpus_file_name);

    PatternModel<uint32_t> _pattern_model;
    _pattern_model = PatternModel<uint32_t>(&_indexed_corpus);
    _pattern_model.train(_corpus_file_name, _pattern_model_options);

    _pattern_model.computestats();
    _pattern_model.computecoveragestats();

    _pattern_model.report(&std::cerr);


    std::cerr << ">" << _pattern_model.totalwordtypesingroup(0,1) << "<";



    _pattern_model.write(_patternmodel_file_name);

    std::cerr << "Some stats, w/e\n" << _indexed_corpus.sentences() << " sentences\n"
        << _pattern_model.types() << " word types\n" << _pattern_model.size() << " pattern types\n" 
        << _pattern_model.tokens() << " word tokens" << std::endl;

    int cntr = 0;

    cpyp::PYPLM<kORDER> lm(_pattern_model.totalwordtypesingroup(0,1), 1, 1, 1, 1);
    //cpyp::PYPLM<kORDER> lm(_pattern_model.types(), 1, 1, 1, 1);
    for(int sample = 0; sample < _samples; ++sample) {
        cntr = 0;
        for( IndexPattern indexPattern : _indexed_corpus) {
            for (Pattern pattern : _pattern_model.getreverseindex(indexPattern.ref)) {
                size_t pattern_size = pattern.size();

                Pattern context = Pattern();
                Pattern focus = Pattern();

                if(pattern_size == kORDER) {
                    if(pattern_size == 1) {
                        focus = pattern[0];
                    } else {
                        context = Pattern(pattern, 0, pattern_size - 1);
                        focus = pattern[pattern_size - 1];
                    }

                    if(sample > 0) {

                        lm.decrement(focus, context, _eng);
                    }
                    lm.increment(focus, context, _eng, &_class_decoder);
                    std::cout << "=========================" << std::endl;
                }
           }
       }

       if(sample % 10 == 9) {
           std::cerr << " [LLH=" << lm.log_likelihood() << "]" << std::endl;
           if(sample % 30u == 29) {
               lm.resample_hyperparameters(_eng);
           }
       } else {
           std::cerr << "." << std::flush;
       }
    }

    //std::ofstream ofile(_output_file.c_str(), std::ios::out | std::ios::binary);

        std::cerr << "Writing LM to " << _serialised_file_name << " ...\n";
        std::ofstream ofile(_serialised_file_name, std::ios::binary);
        if (!ofile.good()) {
            std::cerr << "Failed to open " << _serialised_file_name << " for writing\n";
            return 1;
        }

        boost::archive::binary_oarchive oa(ofile);
        oa << lm;

    std::cerr << "DONE, THANKS!" << std::endl;

    return 0;
}

