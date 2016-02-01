#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"
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

int main(int argc, char** argv) {
    cpyp::MT19937 _eng(1234);

    bool _ignore_errors = true;

    std::cout << "Started at " << giveTime() << std::endl;

    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();


    TrainCommandLineOptions tclo(argc, argv);
    TrainProgramOptions po(tclo, std::stoi(_kORDER));
    PatternModelOptions pmo = TrainPatternModelOptions(tclo.skipgrams, kORDER).patternModelOptions
    CoCoInitialiser cci = CoCoInitialiser(po, pmo, true);

    
    std::ofstream _output;
    std::string outputFilename = po.generalBaseOutputName + ".output";
    _output.open(outputFilename);
    
    std::cout << "Running on " << po.hostName << std::endl;

    _output.close();
} /*
    p2bo("Time: " + _current_time + "\n", _output);

    if(_load_train_vocabulary.empty()) {
        _class_encoder.build(train_input_files, true);
        _class_encoder.save(_class_file_name);
    } else {
        p2bo("Loading " + _load_train_vocabulary + "\n", _output);
        _class_encoder.load(_load_train_vocabulary);
        _class_file_name = _load_train_vocabulary;
    }

    if(_load_train_corpus.empty()) {
        for(auto i:train_input_files) {
            _class_encoder.encodefile(i, _corpus_file_name, 0, 0, 1, 0);
        }
        _class_decoder.load(_class_file_name);
    } else {
        p2bo("Loading " + _load_train_corpus + "\n", _output);
        _corpus_file_name = _load_train_corpus;
    }

    p2bo("Doing corpus stuff\n", _output);
    IndexedCorpus _indexed_corpus = IndexedCorpus(_corpus_file_name);

    p2bo("Doing pattern model stuff\n", _output);
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
            p2bo("Extending the model with " + _extend_model + "\n", _output);
            _pattern_model.load(_extend_model, _pattern_model_options);
            _pattern_model.train(_corpus_file_name, _pattern_model_options, NULL, true, 1, _ignore_errors);

            _pattern_model.write(_patternmodel_file_name);
        } else {
            _pattern_model.load(_load_train_patternmodel, _pattern_model_options);
        }
        
    }

    _pattern_model.computestats();
    _pattern_model.computecoveragestats();

    _pattern_model.report(&std::cerr);

    p2be("Some stats, w/e1\n" + std::to_string(_indexed_corpus.sentences()) + " sentences\n"
        + std::to_string(_pattern_model.totalwordtypesingroup(0, 0)) + " word types\n" + std::to_string(_pattern_model.totalpatternsingroup(0,kORDER)) + " pattern types\n" 
        + std::to_string(_pattern_model.totaltokensingroup(0, 1)) + " word tokens\n", _output);

    int cntr = 0;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"%d-%m-%Y %H:%M:%S", timeinfo);
    _current_time = std::string(buffer);

    std::map<Pattern, int> patternAdded;
    std::map<Pattern, std::vector<Pattern> > patternSpawned;



    p2bo("Time: " + _current_time + "\n", _output);

//    cpyp::PYPLM<kORDER> lm(_pattern_model.totalwordtypesingroup(0,1), 1, 1, 1, 1);
    cpyp::PYPLM<kORDER> lm(_pattern_model.totalwordtypesingroup(0, 0), 1, 1, 1, 1);
    for(int sample = 0; sample < _samples; ++sample) 
    {
        int patterns_processed = 0;
        int patterns_unprocessed = 0;
        for( IndexPattern indexPattern : _indexed_corpus) 
        {
            for (Pattern pattern : _pattern_model.getreverseindex(indexPattern.ref)) 
            {
                if(_pattern_model.has(pattern)) 
                {
                    size_t pattern_size = pattern.size();

                    Pattern context = Pattern();
                    Pattern focus = Pattern();

                    if(pattern_size == 4) // kORDER
                    {
                        if(pattern_size == 1) 
                        {
                            focus = pattern[0];
                        } else 
                        {
                            context = Pattern(pattern, 0, pattern_size - 1);
                            focus = pattern[pattern_size - 1];
                            ++patterns_processed;
                        }

                        if(sample > 0) 
                        {
                            lm.decrement(focus, context, _eng);
                        }
                        lm.increment(focus, context, _eng, nullptr);
  //                      std::cout << "Adding: " << context.tostring(_class_decoder) << " " << focus.tostring(_class_decoder) << std::endl;
                    }
                }
            }

            if(sample % 10 == 9) 
            {
                p2be(" [LLH=" + std::to_string(lm.log_likelihood()) + "]\n", _output);
                if(sample % 30u == 29) 
                {
                    lm.resample_hyperparameters(_eng);
                } else 
                {
                    p2be(".", _output);
                }
            }
        }
    }

    p2be("Writing LM to " + _serialised_file_name + " ...\n", _output);
    std::ofstream ofile(_serialised_file_name, std::ios::binary);
    if (!ofile.good()) 
    {
        p2be("Failed to open " + _serialised_file_name + " for writing\n", _output);
        return 1;
    }

    boost::archive::binary_oarchive oa(ofile);
    oa << lm;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"%d-%m-%Y %H:%M:%S", timeinfo);
    _current_time = std::string(buffer);

    p2bo("Time: " + _current_time + "\n", _output);

    p2be("DONE, THANKS!\n", _output);

    _output.close();

    return 0;
}
*/
