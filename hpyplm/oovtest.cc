#include <iostream>
#include <unordered_map>
#include <cstdlib>

#include <vector>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#define kORDER 3

using namespace std;

int main(int argc, char** argv) {
	string train_input_file = argv[1];
	string test_input_file = argv[2];
	int samples = 2;

	ClassEncoder _class_encoder = ClassEncoder();
	ClassDecoder _class_decoder = ClassDecoder();

	PatternModelOptions _pattern_model_options = PatternModelOptions();
	_pattern_model_options.MAXLENGTH = 3; //kORDER;
	_pattern_model_options.MINLENGTH = 1;
	_pattern_model_options.DOSKIPGRAMS = false;
	_pattern_model_options.DOREVERSEINDEX = true;
	_pattern_model_options.QUIET = true;
	_pattern_model_options.MINTOKENS = 1;

	_class_encoder.build(train_input_file);
	_class_encoder.save("/tmp/tmpout/cpyp.colibri.cls");

	std::string dat_output_file = "/tmp/tmpout/cpyp.colibri.dat";

        _class_encoder.encodefile(train_input_file, dat_output_file, false, false, true, true);

	_class_decoder.load("/tmp/tmpout/cpyp.colibri.cls");

	IndexedCorpus _indexed_corpus = IndexedCorpus(dat_output_file);

	PatternModel<uint32_t> _pattern_model = PatternModel<uint32_t>(&_indexed_corpus);
	_pattern_model.train(dat_output_file, _pattern_model_options, nullptr);

	cerr << "Reading corpus...\n";
	cerr << "E-corpus size: " << _indexed_corpus.sentences() << " sentences\t (" << _pattern_model.types() << " word types, " << _pattern_model.size() << " patterns types and "
			<< _pattern_model.tokens() << " word tokens)\n";


	for (int sample = 0; sample < samples; ++sample) {
		for (IndexPattern it : _indexed_corpus) {
			for (Pattern q : _pattern_model.getreverseindex(it.ref)) {
				size_t p_size = q.size();

				Pattern context = Pattern();
				Pattern focus = Pattern();

				if (p_size == kORDER) {
				    context = Pattern(q, 0, p_size - 1);
				    focus = q[p_size - 1];
				}
			}
		}
	}

	// TESTING

	std::string test_dat_output_file = "/tmp/tmpout/cpyp.test.colibri.dat";

	_class_encoder.encodefile(test_input_file, test_dat_output_file, true, true, false, true);
	_class_encoder.save("/tmp/tmpout/cpyp.test.colibri.cls");

	ClassDecoder _test_class_decoder("/tmp/tmpout/cpyp.test.colibri.cls");

	IndexedCorpus _test_indexed_corpus = IndexedCorpus(test_dat_output_file);

	PatternModel<uint32_t> _test_pattern_model = PatternModel<uint32_t>(&_test_indexed_corpus);
	_test_pattern_model.train(test_dat_output_file, _pattern_model_options, nullptr);

	cerr << "Reading corpus...\n";
	cerr << "E-corpus size: " << _test_indexed_corpus.sentences() << " sentences\t (" << _test_pattern_model.types() << " word types, " << _test_pattern_model.size()
			<< " patterns types and " << _test_pattern_model.tokens() << " word tokens)\n";

        unsigned oovs = 0;

        for (IndexPattern it : _test_indexed_corpus) {
                for (Pattern q : _test_pattern_model.getreverseindex(it.ref)) {
                        size_t p_size = q.size();

                        Pattern context = Pattern();
                        Pattern focus = Pattern();

                        if (p_size == kORDER) {
                                //std::cout << q.tostring(_test_class_decoder) << std::endl;
                                        context = Pattern(q, 0, p_size - 1);
                                        focus = q[p_size - 1];

                                if(!_pattern_model.has(focus)) // OOV if not in the train model
                                //if(!_pattern_model.occurrencecount(focus)) // but both do not work
                                {
                                        ++oovs;
                                }
                        }
                }
        }

        std::cerr << "         OOVs: " << oovs << std::endl;

	std::cout << "Done for now" << std::endl;
	exit(4);

}

