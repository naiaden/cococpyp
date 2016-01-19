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
#include <map>
#include <set>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <sstream>
#include <iomanip>

#include <queue>

#include "cmdline.h"

std::vector<std::string> split(std::string const &input) { 
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer), 
                                 std::istream_iterator<std::string>()};
    return ret;
}

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

    std::string _test_input_directory = clp.get<std::string>("testinput");
    std::string _test_input_file = clp.get<std::string>("testinputfile");
    std::string _input_directory = clp.get<std::string>("trainoutput");
    std::string _output_directory = clp.get<std::string>("testoutput");
    if(clp.get<std::string>("output").empty() && (clp.get<std::string>("testoutput").empty() || clp.get<std::string>("trainoutput").empty())) {
        std::cerr << "Provide either an output, or train and test output directories." << std::endl;
        return -8;
    } else if(!clp.get<std::string>("output").empty() && !clp.get<std::string>("testoutput").empty() && !clp.get<std::string>("trainoutput").empty()) {
        std::cerr << "Ignoring output, but using testoutput and trainoutput." << std::endl;
    } else if(!clp.get<std::string>("output").empty()) {
       _input_directory = clp.get<std::string>("output"); 
       _output_directory = clp.get<std::string>("output");
    }

    std::string _input_run_name = clp.get<std::string>("trainmodel");
    std::string _output_run_name = clp.get<std::string>("testmodel");
    Backoff _backoff_method = fromString(clp.get<std::string>("backoff"));

    std::string _load_corpus = clp.get<std::string>("loadcorpus");
    std::string _load_patternmodel = clp.get<std::string>("loadpatternmodel");
    std::string _load_vocabulary = clp.get<std::string>("loadvocabulary");

//    if(_test_input_directory.empty() && (_load_corpus.empty() || _load_patternmodel.empty() || _load_vocabulary.empty())) {
//        std::cerr << "Not enough arguments to start testing. Double check for either an input directory, or for the proper colibri derivatives." << std::endl;
//        return -8;
//    }

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

    std::vector<std::string> test_input_files;
    if(!_test_input_directory.empty()) {
        boost::filesystem::path foreground_dir(_test_input_directory);
        boost::filesystem::directory_iterator fit(foreground_dir), feod;

        BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod)) {
            if(is_regular_file(p)) {
                test_input_files.push_back(p.string());
            }
        }
    } else {
        test_input_files.push_back(_test_input_file);
    }
    for(auto f: test_input_files)
    {
        std::cout << f << std::endl;
    }
//    } else if(_load_vocabulary.empty() || _load_corpus.empty() || _load_patternmodel.empty()) {
//        std::cerr << "Unexpected situation. Neither test files nor colibri derivatives have been provided!" << std::endl;
//        return -8;
//    }

    std::string _base_input_name = _input_directory + "/" + _input_run_name;
    std::string _input_class_file_name = _base_input_name + ".cls";
    if(!_load_vocabulary.empty()) { _input_class_file_name = _load_vocabulary; }
    std::string _input_corpus_file_name = _base_input_name + ".dat";
    if(!_load_corpus.empty()) { _input_corpus_file_name = _load_corpus; }
    std::string _input_patternmodel_file_name = _base_input_name + ".patternmodel";
    if(!_load_patternmodel.empty()) { _input_patternmodel_file_name = _load_patternmodel; }
    std::string _input_serialised_file_name = _base_input_name + ".ser";


    std::string _general_base_output_name = _output_directory + "/" + _output_run_name + "_" + toString(_backoff_method) + "-common_" + _kORDER;
    std::string _general_output_class_file_name = _general_base_output_name + ".cls";
    std::string _general_output_corpus_file_name = _general_base_output_name + ".dat";
//   std::string _output_patternmodel_file_name = _base_output_name + ".patternmodel";
    //std::string _output_serialised_file_name = _base_output_name + ".ser";
//    std::string _output_probabilities_file_name = _base_output_name + ".probs";

    std::ofstream _general_output;
    std::string _general_output_filename = _general_base_output_name + ".output";
    _general_output.open(_general_output_filename);


    p2bo("Time: " + _current_time + "\n", _general_output);


    _class_encoder.load(_input_class_file_name);

    std::cout << "Ignore 1, just loaded class encoder\n";

    for(auto i : test_input_files) {
        _class_encoder.encodefile(i, _general_output_corpus_file_name, 1, 1, 0, 1);
    }

    std::cout << "Ignore 2, just encoded the files\n";

    _class_encoder.save(_general_output_class_file_name);

    std::cout << "Ignore 3, just saved class encoder\n";

    _class_decoder.load(_general_output_class_file_name);

    std::cout << "Ignore 4, just loaded class decoder\n";

    std::ifstream ifs(_input_serialised_file_name, std::ios::binary);
    if(!ifs.good()) {
        std::cerr << "Something went wrong whilst reading the model: " << _input_serialised_file_name << std::endl;
    }
    boost::archive::binary_iarchive ia(ifs);

    cpyp::PYPLM<kORDER> lm;
    ia & lm;

    
    //IndexedCorpus _indexed_corpus = IndexedCorpus(_input_corpus_file_name);

    PatternSet<uint64_t> allPatterns;
    {
        PatternModel<uint32_t> _train_pattern_model(_input_patternmodel_file_name, _pattern_model_options);
        //PatternModel<uint32_t> _train_pattern_model(_input_patternmodel_file_name, _pattern_model_options, nullptr, &_indexed_corpus);
        allPatterns = _train_pattern_model.extractset();
    }

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
    _current_time = std::string(buffer);

    p2bo("Time: " + _current_time + "\n", _general_output);

/*
    std::map<Pattern, int> patternAdded;
    std::map<Pattern, std::set<Pattern> > patternSpawned;

    for(auto kv : lm.p) {
        std::string p_to_string = kv.first.tostring(_class_decoder);
        if(kv.first.isngram()) {
            std::vector<Pattern> all_subngrams;
            kv.first.subngrams(all_subngrams, kORDER);
            std::set<Pattern> subngrams(all_subngrams.begin(), all_subngrams.end());
           for(gp : subngrams) {
             patternAdded[gp]++;
             patternSpawned[gp].insert(kv.first);
           }
        }
    }
*/
        std::vector<Backoff> all_backoff_options = std::vector<Backoff>();

    if(_backoff_method == Backoff::ALL) {
        all_backoff_options.push_back(Backoff::NGRAM);
        all_backoff_options.push_back(Backoff::BOBACO);
        all_backoff_options.push_back(Backoff::GLM);
    } else 
    {
        all_backoff_options.push_back(_backoff_method);
    }
       for(Backoff i_backoff_method : all_backoff_options) { 


    std::string _base_output_name = _output_directory + "/" + _output_run_name + "_" + toString(i_backoff_method) + "_" + _kORDER;
    std::string _output_class_file_name = _base_output_name + ".cls";
    std::string _output_corpus_file_name = _base_output_name + ".dat";
    std::string _output_patternmodel_file_name = _base_output_name + ".patternmodel";
    //std::string _output_serialised_file_name = _base_output_name + ".ser";
    std::string _output_probabilities_file_name = _base_output_name + ".probs";

    std::ofstream _output;
    std::string _output_filename = _base_output_name + ".output";
    _output.open(_output_filename);

    std::ofstream _probs_file;
    _probs_file.open(_output_probabilities_file_name);

            std::cout << "Processing method " << toString(i_backoff_method) << std::endl;

            time (&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
            _current_time = std::string(buffer);

            p2bo("Time: " + _current_time + "\n", _output);


    double llh = 0;
    unsigned cnt = 0;
    unsigned oovs = 0;
    


            int nr_files = 0;
            int nr_lines = 0;
                std::map<int, int> blabla;
            for(std::string input_file_name : test_input_files)
            {
                    ++nr_files;
        //        std::cout << "Opening file: " << input_file_name << std::endl;
                std::ifstream file(input_file_name);
        //        std::cout << "Opening was succesful" << std::endl;


                std::string retrieved_string;
                while( std::getline(file, retrieved_string))
                {
                        ++nr_lines;
        //            std::cout << "Retrieved line: " << retrieved_string << std::endl;
                    // split sentence
                    std::vector<std::string> words = split(retrieved_string);

        //            std::cout << "Found " << std::to_string(words.size()) << " words" << std::endl;

                    if(words.size() < kORDER)
                    {
                    //    break;
                    } else
                    {
        //                std::cout << "Processing the sentence" << std::endl;
                        // als kORDER = 4, dan is 3 het focuswoord
                        for(int i = (kORDER-1); i < words.size(); ++i)
                        {
        //                    std::cout << "Focus word: " << words[i] << std::endl;
        //                    std::cout << "n=" << kORDER << ", i=" << i << std::endl;
                        
                            std::stringstream context_stream;
                            context_stream << words[i-(kORDER-1)];

                            for(int ii = 1; ii < kORDER-1 ; ++ii)
                            {
                                context_stream << " " << words[i-(kORDER-1)+ii];
                            }

                            Pattern context = _class_encoder.buildpattern(context_stream.str());
                            Pattern focus = _class_encoder.buildpattern(words[i]);

                            double lp;

                            if(i_backoff_method == Backoff::BOBACO) {
                                lp = log(lm.prob(focus, context, nullptr, true, nullptr, nullptr, &blabla)) / log(2);
                            } else if(i_backoff_method == Backoff::GLM) {
                                     if(kORDER == 5) lp = log(lm.j15(focus, context)) / log(2);
                                else if(kORDER == 4) lp = log(lm.j7(focus, context)) / log(2);
                                else if(kORDER == 3) lp = log(lm.j3(focus, context)) / log(2);
                                else lp = log(lm.prob(focus, context, nullptr, false)) / log(2);
                            } else {
                                // baco
                                lp = log(lm.prob(focus, context, nullptr, false)) / log(2);
                            }

                            if(!allPatterns.has(focus)) {
                                ++oovs;
                                lp = 0;
                                _probs_file << "***";
                            }

                            _probs_file << "p(" << focus.tostring(_class_decoder) << " |";
                            _probs_file << context.tostring(_class_decoder) << ") = " << std::fixed << std::setprecision(20) << lp << std::endl;

                            llh -= lp;
                            ++cnt;

                        }
                    }
                }
            }

            for(auto key: blabla)
            {
                std::cout << key.first << " " << key.second << std::endl;
            }

            std::cout << "Processed " << nr_files << " files and " << nr_lines << " lines" << std::endl;

            cnt -= oovs;
            double lprob = (-llh * log(2)) / log(10);
            p2be("  Log_10 prob: " + std::to_string(lprob) + "\n" , _output);
            p2be("        Count: " + std::to_string(cnt) + "\n", _output);
            p2be("         OOVs: " + std::to_string(oovs) + "\n", _output);
            p2be("Cross-Entropy: " + std::to_string((llh / cnt)) + "\n", _output);
            p2be("   Perplexity: " + std::to_string(pow(2, llh / cnt)) + "\n", _output);

            time (&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
            _current_time = std::string(buffer);

            p2bo("Time: " + _current_time + "\n", _output);
    _probs_file.close();
    }

    p2be("Done for now\n" , _general_output);
    _general_output.close();
    

    return 0;

}

