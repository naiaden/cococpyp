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

/*
    1. Directory with training instances
    2. Directory in which we put all stuff, except for the output model
    3. The number of samples
    4. The number of burnin samples (total samples = 4 + 5)
    5. Whether we perform skipgrams
    6. The treshold for ngrams
    7. Name of the run
    8. The treshold for skipgrams
    Optional:
    9. Use this class file
    10. Use this corpus file
    11. Use this pattern model
*/

int main(int argc, char** argv) {
    cpyp::MT19937 _eng;
    
    std::string _train_input_directory = argv[1];
    std::string _output_directory = argv[2];
    int _samples = atoi(argv[3]);
    assert(_samples > 0);
    int _burnin = std::atoi(argv[4]);
    bool _do_skipgrams = (std::atoi(argv[5]) != 0);
    int _min_tokens = std::atoi(argv[6]);
    std::string _run_name = argv[7];
    int _min_skip_tokens = std::atoi(argv[8]);

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

                    //if(cntr++ < 5) {
                    //    std::cerr << std::endl;
                    //    std::cerr << "C: " << context.tostring(_class_decoder) << " -- " << context.hash() << std::endl;
                    //    std::cerr << "F: " << focus.tostring(_class_decoder) << " -- " << focus.hash() << std::endl;
                    //}

                    if(sample > 0) {
                        //std::cerr << std::endl;
                        //std::cerr << "c: " << context.tostring(_class_decoder) << " -- " << context.hash() << std::endl;
                        //std::cerr << "f: " << focus.tostring(_class_decoder) << " -- " << focus.hash() << std::endl;

                        //Pattern rev = context.reverse();
                        //std::cerr << "De reverse: " << rev.tostring(_class_decoder) << std::endl;

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
/*
    {
        std::cerr << "Writing LM to " << _output_file << " ...\n";
        std::ofstream ofile(_output_file.c_str(), std::ios::binary);
        if (!ofile.good()) {
            std::cerr << "Failed to open " << _output_file << " for writing\n";
            return 1;
        }

        boost::archive::binary_oarchive oa(ofile);
        std::string someString = "hoi";
        cpyp::UniformVocabulary uv(100,1,1,1,1);
        cpyp::PYPLM<0> nlm(200,1,1,1,1);
        cpyp::PYPLM<1> olm(400,1,1,1,1);

        std::cerr << ">   " << nlm.log_likelihood() << std::endl;
        std::cerr << ">>  " << olm.log_likelihood() << std::endl;
        std::cerr << ">>> " << lm.log_likelihood() << std::endl;
        std::cerr << ">>> " << lm.log_likelihood() << std::endl;

        oa << someString;
        oa << uv;
        oa << nlm;
        std::cerr << "olm ---------------------------------------" << std::endl;
        oa << olm;
        std::cerr << " lm ---------------------------------------" << std::endl;
        oa << lm;

        ofile.close();
    }

    std::string anotherString;
    cpyp::UniformVocabulary uv1(425,1,1,1,1);
    cpyp::PYPLM<0> nlm1(11,1,1,1,1);
    cpyp::PYPLM<1> olm1;
    cpyp::PYPLM<kORDER> lm1;

    {
        std::cerr << "-   " << nlm1.log_likelihood() << std::endl;
        std::cerr << "--  " << olm1.log_likelihood() << std::endl;
        std::cerr << "--- " << lm1.log_likelihood() << std::endl;

        std::cerr << "Reading from " << _output_file << std::endl;
        std::ifstream ifs(_output_file.c_str(), std::ios::binary);

        if(ifs.good()) {
            std::cerr << "The file was open, or has been opened succesfully!" << std::endl;
        } else {
            std::cerr << "The input archive has not been opened correctly..." << std::endl;
        }

        boost::archive::binary_iarchive ia(ifs);

        std::cerr << "... and that went great. Next step is riskier though..." << std::endl;
   
        std::cerr << "Reading string... ";
        ia >> anotherString;
        std::cerr << "done!\nReading uv1... ";
        ia >> uv1;
        std::cerr << "done!\nReading nlm1... ";
        ia >> nlm1;
        std::cerr << "done!\nReading olm1... ";
        ia >> olm1;
        std::cerr << "done!\nReading lm1... ";
        ia >> lm1;
        std::cerr << "done!\n";

        std::cerr << "+   " << nlm1.log_likelihood() << std::endl;
        std::cerr << "++  " << olm1.log_likelihood() << std::endl;
        std::cerr << "+++ " << lm1.log_likelihood() << std::endl;
    }
    */

