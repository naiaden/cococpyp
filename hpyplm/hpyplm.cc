#include <iostream>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"
#include "corpus/corpus.h"
#include "cpyp/m.h"
#include "cpyp/random.h"
#include "cpyp/crp.h"
#include "cpyp/tied_parameter_resampler.h"

#include <vector>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <sstream>

#include "cmdline.h"

enum class Backoff { GLM, BOBACO, NGRAM };

Backoff fromString(const std::string& s) {
    if(s.compare("glm") == 0) return Backoff::GLM;
    else if(s.compare("bobaco") == 0) return Backoff::BOBACO;
    else return Backoff::NGRAM;
}

std::string toString(Backoff b) {
    if(b == Backoff::GLM) return "GLM";
    if(b == Backoff::BOBACO) return "bobaco";
    if(b == Backoff::NGRAM) return "ngram";
    return "unknown backoff method";
}

int main(int argc, char** argv) {
  
    std::stringstream oss;
    oss << kORDER;
    //int _kORDER = std::atoi(oss.str().c_str());
    std::string _kORDER = oss.str();

    cmdline::parser clp;

    clp.add<std::string>("traininput", 'i', "train input directory", false, "");
    clp.add<std::string>("testinput", 'I', "test input directory", false, "");
    clp.add<std::string>("output", 'o', "output directory", true, "");
 
    clp.add<int>("samples", 's', "samples", false, 50);
    clp.add<int>("burnin", 'b', "burnin", false, 0);
    clp.add<std::string>("backoff", 'B', "the backoff method", false, "ngram", cmdline::oneof<std::string>("glm", "bobaco", "ngram"));

    clp.add("skipgram", 'S', "train with skipgrams");
    clp.add<int>("streshold", 'T', "treshold for skipgrams", false, 1);
    clp.add<int>("treshold", 't', "treshold for ngrams", false, 1);

    clp.add<std::string>("modelname", 'm', "the name of the training model", true);

    clp.add<int>("reportppl", 'p', "report ppl every nth iteration", false, 0);

    clp.add<std::string>("loadtraincorpus", '\0' , "load colibri encoded corpus (train)", false, "");
    clp.add<std::string>("loadtrainpatternmodel", '\0', "load colibri encoded pattern model (train)", false, "");
    clp.add<std::string>("loadtrainvocabulary", '\0', "load colibri class file (train)", false, "");

    clp.add<std::string>("loadtestcorpus", '\0', "load colibri encoded corpus (test)", false, "");
    clp.add<std::string>("loadtestpatternmodel", '\0', "load colibri encoded pattern model (test)", false, "");
    clp.add<std::string>("loadtestvocabulary", '\0', "load colibri class file (test)", false, "");

    clp.add<int>("seed", 'R', "initialise with random seed", false, 0);

    clp.parse_check(argc, argv);

    cpyp::MT19937 _eng;
    if(clp.get<int>("seed") > 0) {
        _eng = cpyp::MT19937(clp.get<int>("seed"));
    }

    std::string _train_input_directory = clp.get<std::string>("traininput");
    std::string _test_input_directory = clp.get<std::string>("testinput");
    std::string _output_directory = clp.get<std::string>("output");

    int _samples = clp.get<int>("samples");
    int _burnin = clp.get<int>("burnin");

    bool _do_skipgrams = clp.exist("skipgram");
    int _min_skip_tokens = clp.get<int>("streshold");
    int _min_tokens = clp.get<int>("treshold");

    std::string _run_name = clp.get<std::string>("modelname");
    int _report_ppl = clp.get<int>("reportppl");

    Backoff _backoff_method = fromString(clp.get<std::string>("backoff"));
    std::cerr << "Using backoff method: " << toString(_backoff_method) << std::endl;

    std::string _load_train_corpus = clp.get<std::string>("loadtraincorpus");
    std::string _load_train_patternmodel = clp.get<std::string>("loadtrainpatternmodel");
    std::string _load_train_vocabulary = clp.get<std::string>("loadtrainvocabulary");

    std::string _load_test_corpus = clp.get<std::string>("loadtestcorpus");
    std::string _load_test_patternmodel = clp.get<std::string>("loadtestpatternmodel");
    std::string _load_test_vocabulary = clp.get<std::string>("loadtestvocabulary");

    if(_train_input_directory.empty() && (_load_train_corpus.empty() || _load_train_patternmodel.empty() || _load_train_vocabulary.empty())) {
        std::cerr << "Not enough arguments to start training. Double check for either an input directory, or for the proper colibri derivatives." << std::endl;
        return -8;
    }

    if(_test_input_directory.empty() && (_load_test_corpus.empty() || _load_test_patternmodel.empty() || _load_test_vocabulary.empty())) {
        std::cerr << "Not enough arguments to start testing. Double check for either an input directory, or for the proper colibri derivatives." << std::endl;
        return -8;
    }

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

    PatternModelOptions _test_pattern_model_options = PatternModelOptions();
    _test_pattern_model_options.MAXLENGTH = kORDER;
    _test_pattern_model_options.MINLENGTH = 1;
    _test_pattern_model_options.DOSKIPGRAMS = false;//_do_skipgrams;
    _test_pattern_model_options.DOSKIPGRAMS_EXHAUSTIVE = false;//_do_skipgrams;
    _test_pattern_model_options.DOREVERSEINDEX = true;
    _test_pattern_model_options.QUIET = false;
    _test_pattern_model_options.MINTOKENS = 1;

    // +train

    std::vector<std::string> train_input_files;
    if(!_train_input_directory.empty()) {
        boost::filesystem::path background_dir(_train_input_directory);
        boost::filesystem::directory_iterator bit(background_dir), beod;

        BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, beod)){
            if(is_regular_file(p)/* && p.extension() == ".txt"*/)
            {
                    train_input_files.push_back(p.string());
            }
        }
    } else if(_load_train_vocabulary.empty() || _load_train_corpus.empty() || _load_train_patternmodel.empty()) {
        std::cerr << "Unexpected situation. Neither training files nor colibri derivatives have been provided!" << std::endl;
        return -8;
    }

    std::string _train_base_name = _output_directory + "/" + _run_name + "_" + toString(_backoff_method) + "_" + _kORDER + (_do_skipgrams ? "S" : "") + "_train";
    std::string _train_class_file_name = _train_base_name + ".cls";
    std::string _train_corpus_file_name = _train_base_name + ".dat";
    std::string _train_patternmodel_file_name = _train_base_name + ".patternmodel";
    std::string _train_serialised_file_name = _train_base_name + ".ser";


    if(_load_train_vocabulary.empty()) {
        _class_encoder.build(train_input_files, true);
        _class_encoder.save(_train_class_file_name);
    } else {
        _class_encoder.load(_load_train_vocabulary);
        _train_class_file_name = _load_train_vocabulary;
    }

    if(_load_train_corpus.empty()) {
        for (auto i : train_input_files) {
            _class_encoder.encodefile(i, _train_corpus_file_name, false, false, true, false);
        }
        _class_decoder.load(_train_class_file_name);
    } else {
        _train_corpus_file_name = _load_train_corpus;
    }
    
    
    IndexedCorpus _indexed_corpus = IndexedCorpus(_train_corpus_file_name);

    PatternModel<uint32_t> _pattern_model(&_indexed_corpus);

    if(_load_train_patternmodel.empty()) {
        _pattern_model.train(_train_corpus_file_name, _pattern_model_options, nullptr);

        _pattern_model.write(_train_patternmodel_file_name);
    } else {
        _pattern_model.load(_load_train_patternmodel, _pattern_model_options, nullptr);
    }

    _pattern_model.computestats();
    _pattern_model.computecoveragestats();

    _pattern_model.report(&std::cerr);

    std::cerr << ">" << _pattern_model.totalwordtypesingroup(0,1) << "<";



    std::cerr << "Some stats, w/e\n" << _indexed_corpus.sentences() << " sentences\n"
        << _pattern_model.types() << " word types\n" << _pattern_model.size() << " pattern types\n" 
        << _pattern_model.tokens() << " word tokens" << std::endl;	
	
    // -train
    // +test

    std::vector<std::string> test_input_files;
    if(!_test_input_directory.empty()) {
        boost::filesystem::path foreground_dir(_test_input_directory);
        boost::filesystem::directory_iterator fit(foreground_dir), feod;

        BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod)) {
            if(is_regular_file(p)) {
                test_input_files.push_back(p.string());
            }
        }
    } else if(_load_test_vocabulary.empty() || _load_test_corpus.empty() || _load_train_patternmodel.empty()) {
        std::cerr << "Unexpected situation. Neither test files nor colibri derivatives have been provided!" << std::endl;
        return -8;
    }

    std::string _test_base_output_name = _output_directory + "/" + _run_name + "_" + toString(_backoff_method) + "_" + _kORDER + "_test";
    std::string _test_output_class_file_name = _test_base_output_name + ".cls";
    std::string _test_output_corpus_file_name = _test_base_output_name + ".dat";
    std::string _test_output_patternmodel_file_name = _test_base_output_name + ".patternmodel";
    std::string _test_output_probabilities_file_name = _test_base_output_name + ".probs";
    std::string _test_output_perplexities_file_name = _test_base_output_name + ".ppl";

    if(_load_test_vocabulary.empty()) {
        for(auto i : test_input_files) {
            _class_encoder.encodefile(i, _test_output_corpus_file_name, 1, 1, 0, 1);
        }
        _class_encoder.save(_test_output_class_file_name);
    } else {
        _class_encoder.load(_load_test_vocabulary);
        _test_output_class_file_name = _load_test_vocabulary;
    }

    ClassDecoder _test_class_decoder = ClassDecoder();
    _test_class_decoder.load(_test_output_class_file_name);


    IndexedCorpus _test_indexed_corpus = IndexedCorpus(_test_output_corpus_file_name);

    PatternModel<uint32_t> _test_pattern_model(&_test_indexed_corpus);
    if(_load_test_patternmodel.empty()) {
        _test_pattern_model.train(_test_output_corpus_file_name, _test_pattern_model_options);

        _test_pattern_model.write(_test_output_patternmodel_file_name);    
    } else {
        _test_pattern_model.load(_load_test_patternmodel, _test_pattern_model_options, nullptr);
    }

    _test_pattern_model.computestats();
    _test_pattern_model.computecoveragestats();

    _test_pattern_model.report(&std::cerr);
    std::cerr << "Test>" << _test_pattern_model.totalwordtypesingroup(0,1) << "<";

    std::cerr << "Some stats, w/e\n" << _test_indexed_corpus.sentences() << " sentences\n"
        << _test_pattern_model.types() << " word types\n" << _test_pattern_model.size() << " pattern types\n" 
        << _test_pattern_model.tokens() << " word tokens" << std::endl;	

    // -test

    std::ofstream _ppl_file;
    _ppl_file.open(_test_output_perplexities_file_name);

    std::ofstream _probs_file;
    _probs_file.open(_test_output_probabilities_file_name);

    std::cerr << "Found " << _pattern_model.types() << " training types and " << 
        _test_pattern_model.types() << " testing types" << std::endl;
    std::cerr << "Performing " << _samples << " samples" << std::endl;

    cpyp::PYPLM<kORDER> lm(_pattern_model.types(), 1, 1, 1, 1);
    for (int sample = 0; sample < _samples; ++sample) {
        for (IndexPattern it : _indexed_corpus) {
                for (Pattern q : _pattern_model.getreverseindex(it.ref)) {

                       // std::cout << q.tostring(_test_class_decoder) << std::endl;

                        size_t p_size = q.size();

                        Pattern context = Pattern();
                        Pattern focus = Pattern();

                        if (p_size == kORDER) {
                                if (p_size == 1) {
                                        focus = q[0];
                                } else {
                                        context = Pattern(q, 0, p_size - 1);
                                        focus = q[p_size - 1];
                                }

                                if (sample > 0) {
                                        lm.decrement(focus, context, _eng);
                                }
                                lm.increment(focus, context, _eng, nullptr);
                        }
                }
        }

        // end training

        if (_report_ppl && sample % _report_ppl == 0) {
            double llh = 0;
            unsigned cnt = 0;
            unsigned oovs = 0;

            for (IndexPattern it : _test_indexed_corpus) {
                    for (Pattern q : _test_pattern_model.getreverseindex(it.ref)) {
                            size_t p_size = q.size();
            
                            Pattern context = Pattern();
                            Pattern focus = Pattern();
            
                            if (p_size == kORDER) {
                                    context = Pattern(q, 0, p_size - 1);
                                    focus = q[p_size - 1];
            
                                    if (p_size == 1) {
                                            focus = q[0];
                                    } else {
                                            context = Pattern(q, 0, p_size - 1);
                                            focus = q[p_size - 1];
                                    }
            
                                    double lp;
                                    if(_backoff_method == Backoff::BOBACO) {
                                        lp = log(lm.prob(focus, context, nullptr, true)) / log(2);
                                    } else if(_backoff_method == Backoff::GLM) {
                                             if(kORDER == 5) lp = log(lm.j15(focus, context)) / log(2);
                                        else if(kORDER == 4) lp = log(lm.j7(focus, context)) / log(2);
                                        else if(kORDER == 3) lp = log(lm.j3(focus, context)) / log(2);
                                        else lp = log(lm.prob(focus, context, nullptr, false)) / log(2);
                                    } else {
                                        // baco
                                        lp = log(lm.prob(focus, context, nullptr, false)) / log(2);
                                    }

                                    if (!_test_pattern_model.has(focus)) // OOV if not in the train model
                                    {
                                            ++oovs;
                                            lp = 0;
                                    }
                                    llh -= lp;
                                    ++cnt;
                            }
                    }
            }

            cnt -= oovs;
            _ppl_file << "Sample: " << sample << "\tPerplexity: " << pow(2, llh / cnt) << std::endl;

        } 
        
        if (!_report_ppl && sample % 10 == 9) {
                std::cerr << " [LLH=" << lm.log_likelihood() << "]" << std::endl;
                if (sample % 30u == 29)
                        lm.resample_hyperparameters(_eng);
        } else {
                std::cerr << '.' << std::flush;
        }



    }

    double llh = 0;
    unsigned cnt = 0;
    unsigned oovs = 0;

    for (IndexPattern it : _test_indexed_corpus) {
            for (Pattern q : _test_pattern_model.getreverseindex(it.ref)) {
                    size_t p_size = q.size();
    
                    Pattern context = Pattern();
                    Pattern focus = Pattern();
    
                    if (p_size == kORDER) {
                            context = Pattern(q, 0, p_size - 1);
                            focus = q[p_size - 1];
    
                            if (p_size == 1) {
                                    focus = q[0];
                            } else {
                                    context = Pattern(q, 0, p_size - 1);
                                    focus = q[p_size - 1];
                            }

                            double lp;
                            if(_backoff_method == Backoff::BOBACO) {
                                lp = log(lm.prob(focus, context, nullptr, true)) / log(2);
                            } else if(_backoff_method == Backoff::GLM) {
                                     if(kORDER == 5) lp = log(lm.j15(focus, context)) / log(2);
                                else if(kORDER == 4) lp = log(lm.j7(focus, context)) / log(2);
                                else if(kORDER == 3) lp = log(lm.j3(focus, context)) / log(2);
                                else lp = log(lm.prob(focus, context, nullptr, false)) / log(2);
                            } else {
                                // baco
                                lp = log(lm.prob(focus, context, nullptr, false)) / log(2);
                            }

                            if (!_test_pattern_model.has(focus)) // OOV if not in the train model
                            {
                                    ++oovs;
                                    lp = 0;
                            }
                            llh -= lp;
                            ++cnt;
    
                            _probs_file << "p[" << _pattern_model.occurrencecount(focus) << "](" << focus.tostring(_class_decoder) << " |";
                            _probs_file << context.tostring(_test_class_decoder) << ") = " << lp << std::endl;

                            llh -= lp;
                            ++cnt;
    
                    } else {
                            //std::cout << "Skipping: " << q.tostring(_class_decoder) << std::endl;
                    }
            }
    }

    _ppl_file.close();
    _probs_file.close();

    cnt -= oovs;
    std::cerr << "  Log_10 prob: " << (-llh * log(2) / log(10)) << std::endl;
    std::cerr << "        Count: " << cnt << std::endl;
    std::cerr << "         OOVs: " << oovs << std::endl;
    std::cerr << "Cross-Entropy: " << (llh / cnt) << std::endl;
    std::cerr << "   Perplexity: " << pow(2, llh / cnt) << std::endl;

    std::cerr << "Done for now" << std::endl;
    exit(4);

}
