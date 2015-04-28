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

#include <sstream>
#include <iomanip>

#include "cmdline.h"
#include "mem.hpp"

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

    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string _current_time(buffer);

    
    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();

    cmdline::parser clp;

    clp.add<std::string>("output", '\0', "train and test output directory", false, "");
    clp.add<std::string>("trainoutput", 'O', "train output directory", false, "");

    clp.add<std::string>("trainmodel", 'm', "the name of the training model", true);

    clp.add<std::string>("loadcorpus", '\0', "load colibri encoded corpus", false, "");
    clp.add<std::string>("loadpatternmodel", '\0', "load colibri encoded pattern model", false, "");
    clp.add<std::string>("loadvocabulary", '\0', "load colibri class file", false, "");

    clp.parse_check(argc, argv);

    std::string _input_directory = clp.get<std::string>("trainoutput");

    std::string _input_run_name = clp.get<std::string>("trainmodel");

    std::string _load_corpus = clp.get<std::string>("loadcorpus");
    std::string _load_patternmodel = clp.get<std::string>("loadpatternmodel");
    std::string _load_vocabulary = clp.get<std::string>("loadvocabulary");

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

    PatternModelOptions _pattern_model_options = PatternModelOptions();
    _pattern_model_options.MAXLENGTH = kORDER;
    _pattern_model_options.MINLENGTH = 1; // kORDER - 1;
    _pattern_model_options.DOSKIPGRAMS = false;
    _pattern_model_options.DOSKIPGRAMS_EXHAUSTIVE = false;
    _pattern_model_options.DOREVERSEINDEX = true;
    _pattern_model_options.QUIET = false;
    _pattern_model_options.MINTOKENS = 1;

    std::string _base_input_name = _input_directory + "/" + _input_run_name;
    std::string _input_class_file_name = _base_input_name + ".cls";
    std::string _input_corpus_file_name = _base_input_name + ".dat";
    std::string _input_patternmodel_file_name = _base_input_name + ".patternmodel";
    std::string _input_serialised_file_name = _base_input_name + ".ser";

    std::string _base_output_name = "analysis_" + _input_directory + "/" + _input_run_name + "_" + _kORDER;

    std::ofstream _output;
    std::string _output_filename = _base_output_name + ".output";
    _output.open(_output_filename);

    p2bo("Time: " + _current_time + "\n", _output);

    std::cout << currentRSSToString() << std::endl;
    //p2bo("Memory usage: " + getCurrentRSS(), _output);

    _class_encoder.load(_input_class_file_name);
    _class_decoder.load(_input_class_file_name);

    IndexedCorpus _train_indexed_corpus = IndexedCorpus(_input_corpus_file_name);

//    std::cout << "Ignore 5, just created an indexed corpus\n";

    PatternModel<uint32_t> _train_pattern_model = PatternModel<uint32_t>(&_train_indexed_corpus);

//    std::cout << "Ignore 6, just created a pattern model\n";

    _train_pattern_model.train(_input_corpus_file_name, _pattern_model_options);

//    std::cout << "Ignore 7, just trained pattern model\n";

    _train_pattern_model.computestats();
    _train_pattern_model.computecoveragestats();

//    std::cout << "Ignore 8, just computed stuff\n";

    _train_pattern_model.write(_input_patternmodel_file_name);
    
//    std::cout << "Ignore 9, just write the model to a file\n";

    double llh = 0;
    unsigned cnt = 0;
    unsigned oovs = 0;
    
    std::ifstream ifs(_input_serialised_file_name, std::ios::binary);
    if(!ifs.good()) {
        std::cerr << "Something went wrong whilst reading the model: " << _input_serialised_file_name << std::endl;
    }
    boost::archive::binary_iarchive ia(ifs);

    std::cout << currentRSSToString() << std::endl;
    //p2bo("Memory usage: " + getCurrentRSS(), _output);

    cpyp::PYPLM<kORDER> lm;
    ia & lm;

    PatternSet<uint64_t> allPatterns;
    {
        PatternModel<uint32_t> _train_pattern_model(_input_patternmodel_file_name, _pattern_model_options);
        allPatterns = _train_pattern_model.extractset();
    }

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
    _current_time = std::string(buffer);

    p2bo("Time: " + _current_time + "\n", _output);

    p2be("Done for now\n" , _output);
    _output.close();
    
    std::cout << currentRSSToString() << std::endl;
    //p2bo("Memory usage: " + getCurrentRSS(), _output);

    return 0;
}

