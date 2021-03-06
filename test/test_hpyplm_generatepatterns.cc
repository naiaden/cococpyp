#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm/hpyplm.h"
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
#include <map>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <sstream>
#include <iomanip>

#include "hpyplm/uniform_vocab.h"
#include "cmdline.h"

void p2b(const std::string& s, std::ostream& os, std::ofstream& ofs) {

    ofs << s;
    os << s;
}

void p2bo(const std::string& s, std::ofstream& ofs) {
    p2b(s, std::cout, ofs);
}

void p2be(const std::string& s, std::ofstream& ofs) {
    p2b(s, std::cerr, ofs);
}

int main(int argc, char** argv) {

    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();

    cmdline::parser clp;

    clp.add<std::string>("traininput", 'i', "train input directory", false);
    clp.add<std::string>("traininputfile", 'f', "train input file", false);
    clp.add<std::string>("trainoutput", 'o', "train output directory", true);
    
    clp.add("skipgram", 'S', "train with skipgrams");
    clp.add<int>("streshold", 'T', "treshold for skipgrams", false, 1);
    clp.add<int>("treshold", 't', "treshold for ngrams", false, 1);
    clp.add<int>("unigramtreshold", 'W', "unigram treshold", false, 1);
    clp.add<int>("prunedonsubsumed", 'p', "prune all n-grams that are not subsumed by higher order n-grams", false, 0);

    clp.add<std::string>("modelname", 'm', "the name of the training model", true);

    clp.add<std::string>("loadtraincorpus", '\0', "load colibri encoded corpus", false, "");
    clp.add<std::string>("loadtrainpatternmodel", '\0', "load colibri encoded pattern model", false, "");
    clp.add<std::string>("loadtrainvocabulary", '\0', "load colibri class file", false, "");
    clp.add<std::string>("extendmodel", 'E', "extend current model (with larger n or skips)", false, "");

    clp.parse_check(argc, argv);

    std::string _train_input_file = clp.get<std::string>("traininputfile");

    std::string _train_input_directory = clp.get<std::string>("traininput");
    std::string _output_directory = clp.get<std::string>("trainoutput");

    bool _do_skipgrams = clp.exist("skipgram");
    int _min_skip_tokens = clp.get<int>("streshold");
    int _min_tokens = clp.get<int>("treshold");
    int _unigram_treshold = clp.get<int>("unigramtreshold");
    int _pruned_on_subsume = clp.get<int>("prunedonsubsumed");

    std::string _run_name = clp.get<std::string>("modelname");

    std::string _load_train_corpus = clp.get<std::string>("loadtraincorpus");
    std::string _load_train_patternmodel = clp.get<std::string>("loadtrainpatternmodel");
    std::string _load_train_vocabulary = clp.get<std::string>("loadtrainvocabulary");
    std::string _extend_model = clp.get<std::string>("extendmodel");

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

    PatternModelOptions _pattern_model_options = PatternModelOptions();
    _pattern_model_options.MAXLENGTH = kORDER;
    _pattern_model_options.MINLENGTH = 1;
    _pattern_model_options.DOSKIPGRAMS = _do_skipgrams;
    _pattern_model_options.DOSKIPGRAMS_EXHAUSTIVE = _do_skipgrams;
    _pattern_model_options.DOREVERSEINDEX = true;
    _pattern_model_options.QUIET = false;
    _pattern_model_options.MINTOKENS = _min_tokens;
    _pattern_model_options.MINTOKENS_SKIPGRAMS = _min_skip_tokens;
    _pattern_model_options.MINTOKENS_UNIGRAMS = _unigram_treshold;
    _pattern_model_options.PRUNENONSUBSUMED = _pruned_on_subsume;

    
    std::vector<std::string> train_input_files;
    if(!_train_input_directory.empty()) {
        boost::filesystem::path background_dir(_train_input_directory);
        boost::filesystem::directory_iterator bit(background_dir), beod;
    
        BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, beod)) {
            if(is_regular_file(p)) {
                train_input_files.push_back(p.string());
            }
        }
    } else {
       train_input_files.push_back(_train_input_file); 
    }
    
    std::string _base_name = _output_directory + "/" + _run_name + "_" + _kORDER + (_do_skipgrams ? "S" : "") + "_W" + std::to_string(_unigram_treshold) + "_t" + std::to_string(_min_tokens) + "_T" + std::to_string(_min_skip_tokens) +  "_p" + std::to_string(_pruned_on_subsume) + "_train";
    std::string _class_file_name = _base_name + ".cls";
    std::string _corpus_file_name = _base_name + ".dat";
    std::string _patternmodel_file_name = _base_name + ".patternmodel";
    
    if(_load_train_vocabulary.empty()) {
        _class_encoder.build(train_input_files, true);
        _class_encoder.save(_class_file_name);
    } else {
        _class_encoder.load(_load_train_vocabulary);
        _class_file_name = _load_train_vocabulary;
    }

    if(_load_train_corpus.empty()) {
        for(auto i:train_input_files) {
            _class_encoder.encodefile(i, _corpus_file_name, 0, 0, 1, 0);
        }
        _class_decoder.load(_class_file_name);
    } else {
        _corpus_file_name = _load_train_corpus;
    }

    IndexedCorpus _indexed_corpus = IndexedCorpus(_corpus_file_name);

    PatternModel<uint32_t> _pattern_model(&_indexed_corpus);

    if(_extend_model.empty())
    {
        if(_load_train_patternmodel.empty()) {
            _pattern_model.train(_corpus_file_name, _pattern_model_options);

            _pattern_model.write(_patternmodel_file_name);
        } else {
            _pattern_model.load(_load_train_patternmodel, _pattern_model_options);
        }
    } else
    {
        if(_load_train_patternmodel.empty()) {
            _pattern_model.load(_extend_model, _pattern_model_options);
            _pattern_model.train(_corpus_file_name, _pattern_model_options, NULL, true, 1, false);

            _pattern_model.write(_patternmodel_file_name);
        } else {
            _pattern_model.load(_load_train_patternmodel, _pattern_model_options);
        }
        
    }

    _pattern_model.computestats();
    _pattern_model.computecoveragestats();

//    _pattern_model.report(&std::cerr);



    return 0;
}


