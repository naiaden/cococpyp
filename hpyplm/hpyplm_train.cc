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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <vector>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include "hpyplm/uniform_vocab.h"

/*
    1. Directory with training instances
    2. Name of the output model
    3. Directory in which we put all stuff, except for the output model
    4. The number of samples
    5. The number of burnin samples (total samples = 4 + 5)
    6. Whether we perform skipgrams
    7. The minimal number of tokens
    Optional:
    8. Use this class file
    9. Use this corpus file
    10. Use this pattern model
*/

int main(int argc, char** argv) {
    cpyp::MT19937 _eng;
    
    std::string _train_input_directory = argv[1];
    std::string _output_file = argv[2];
    {
        std::ifstream test(_output_file);
        if (test.good()) {
            std::cerr << "File " << _output_file << " appears to exist: please remove\n";
            return 1;
        }
    }
    std::string _output_directory = argv[3];
    int _samples = atoi(argv[4]);
    assert(_samples > 0);
    int _burnin = std::atoi(argv[5]);
    bool _do_skipgrams = (std::atoi(argv[6]) != 0);
    int _min_tokens = std::atoi(argv[7]);

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

    PatternModelOptions _pattern_model_options = PatternModelOptions();
    _pattern_model_options.MAXLENGTH = kORDER;
    _pattern_model_options.MINLENGTH = kORDER - 1;
    _pattern_model_options.DOSKIPGRAMS = _do_skipgrams;
    _pattern_model_options.DOSKIPGRAMS_EXHAUSTIVE = _do_skipgrams;
    _pattern_model_options.DOREVERSEINDEX = true;
    _pattern_model_options.QUIET = false;
    _pattern_model_options.MINTOKENS = _min_tokens;

    boost::filesystem::path background_dir(_train_input_directory);
    boost::filesystem::directory_iterator bit(background_dir), beod;

    std::vector<std::string> train_input_files;
    BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, beod)) {
        if(is_regular_file(p)) {
            train_input_files.push_back(p.string());
        }
    }

    _class_encoder.build(train_input_files, true);
    _class_encoder.save(_output_directory + "/" + "justsomemodel" + ".cls");

    for(auto i:train_input_files) {
        _class_encoder.encodefile(i, _output_directory + "/" + "justsomemodel" + ".dat", 0, 0, 1, 0);
    }
    _class_decoder.load(_output_directory + "/" + "justsomemodel" + ".cls");

    IndexedCorpus _indexed_corpus = IndexedCorpus(_output_directory + "/" + "justsomemodel" + ".dat");

    PatternModel<uint32_t> _pattern_model;
    _pattern_model = PatternModel<uint32_t>(&_indexed_corpus);
    _pattern_model.train(_output_directory + "/" + "justsomemodel" + ".dat", _pattern_model_options);

    _pattern_model.computestats();
    _pattern_model.computecoveragestats();

    _pattern_model.report(&std::cerr);


    std::cerr << ">" << _pattern_model.totalwordtypesingroup(0,1) << "<";



    _pattern_model.write(_output_directory + "/" + "justsomemodel" + ".patternmodel");

    std::cerr << "Some stats, w/e\n" << _indexed_corpus.sentences() << " sentences\n"
        << _pattern_model.types() << " word types\n" << _pattern_model.size() << " pattern types\n" 
        << _pattern_model.tokens() << " word tokens" << std::endl;


    cpyp::PYPLM<kORDER> lm(359, 1, 1, 1, 1);
    //cpyp::PYPLM<kORDER> lm(_pattern_model.types(), 1, 1, 1, 1);
    for(int sample = 0; sample < _samples; ++sample) {
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
                    lm.increment(focus, context, _eng);
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

    std::cerr << "Writing LM to " << _output_file << " ...\n";
    std::ofstream ofile(_output_file.c_str(), std::ios::out | std::ios::binary);
    if (!ofile.good()) {
        std::cerr << "Failed to open " << _output_file << " for writing\n";
        return 1;
    }

{
    boost::archive::text_oarchive oa(ofile);
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
    oa << olm;
    oa << lm;
}
    std::string anotherString;
    cpyp::UniformVocabulary uv1(425,1,1,1,1);
    cpyp::PYPLM<0> nlm1(11,1,1,1,1);
    cpyp::PYPLM<1> olm1;
    cpyp::PYPLM<kORDER> lm1;

    std::cerr << "-   " << nlm1.log_likelihood() << std::endl;
    std::cerr << "--  " << olm1.log_likelihood() << std::endl;
    std::cerr << "--- " << lm1.log_likelihood() << std::endl;

    std::cerr << "Reading from " << _output_file << std::endl;
    std::ifstream ifs(_output_file);
    boost::archive::text_iarchive ia(ifs);

    ia >> anotherString;
    ia >> uv1;
    ia >> nlm1;
    ia >> olm1;
    ia >> lm1;

    std::cerr << "+   " << nlm1.log_likelihood() << std::endl;
    std::cerr << "++  " << olm1.log_likelihood() << std::endl;
    std::cerr << "+++ " << lm1.log_likelihood() << std::endl;

    return 0;
}

