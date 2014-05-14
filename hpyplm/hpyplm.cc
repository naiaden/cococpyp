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

#define kORDER 3

using namespace std;
using namespace cpyp;

int main(int argc, char** argv) {
	if (argc != 4) {
		cerr << argv[0] << " <training_dir> <test_dir> <nsamples>\n\nEstimate a " << kORDER << "-gram HPYP LM and report perplexity\n100 is usually sufficient for <nsamples>\n";
		return 1;
	}
	MT19937 eng(123456);
	string train_input_directory = argv[1];
	string test_input_directory = argv[2];
	int samples = atoi(argv[3]);

	ClassEncoder _class_encoder = ClassEncoder();
	ClassDecoder _class_decoder = ClassDecoder();

	PatternModelOptions _pattern_model_options = PatternModelOptions();
	_pattern_model_options.MAXLENGTH = 3; //kORDER;
	_pattern_model_options.MINLENGTH = kORDER;
	_pattern_model_options.DOSKIPGRAMS = false;
	_pattern_model_options.DOREVERSEINDEX = true;
	_pattern_model_options.QUIET = true;
	_pattern_model_options.MINTOKENS = 1;

	boost::filesystem::path background_dir(train_input_directory);
	boost::filesystem::directory_iterator bit(background_dir), beod;

	std::vector<std::string> train_input_files;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(bit, beod)){
	if(is_regular_file(p) && p.extension() == ".txt")
	{
		train_input_files.push_back(p.string());
	}
}

	std::cout << "Found " << train_input_files.size() << " files" << std::endl;

	_class_encoder.build(train_input_files, true);
	_class_encoder.save("/tmp/tmpout/cpyp.colibri.cls");

	std::string dat_output_file = "/tmp/tmpout/cpyp.colibri.dat";

	for (auto i : train_input_files) {
		_class_encoder.encodefile(i, dat_output_file, false, false, true, true);
	}

	_class_decoder.load("/tmp/tmpout/cpyp.colibri.cls");

	IndexedCorpus _indexed_corpus = IndexedCorpus(dat_output_file);

	PatternModel<uint32_t> _pattern_model = PatternModel<uint32_t>(&_indexed_corpus);
	_pattern_model.train(dat_output_file, _pattern_model_options, nullptr);

	_pattern_model.computestats();
	_pattern_model.computecoveragestats();

	std::cout << ">> maxn:" << _pattern_model.maxlength() << std::endl;

	cerr << "Reading corpus...\n";
	cerr << "E-corpus size: " << _indexed_corpus.sentences() << " sentences\t (" << _pattern_model.types() << " word types, " << _pattern_model.size() << " patterns types and "
			<< _pattern_model.tokens() << " word tokens)\n";

	int increments = 0;

	PYPLM<kORDER> lm(_pattern_model.types(), 1, 1, 1, 1);
	for (int sample = 0; sample < samples; ++sample) {
		int decrements = 0;
		for (IndexPattern it : _indexed_corpus) {
			for (Pattern q : _pattern_model.getreverseindex(it.ref)) {
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

					ClassDecoder* cd = nullptr;

					if (sample > 0) {
						std::cout << focus.tostring(_class_decoder) << " -- " << context.tostring(_class_decoder) << std::endl;
						cd = &_class_decoder;
						std::cout << "\tDecrementing: " << decrements << std::endl;
						lm.decrement(focus, context, eng, cd);
						++decrements;
					}
                                        std::cout << "\tIncrementing" << std::endl;
					lm.increment(focus, context, eng, &_class_decoder);
				} else {
					//std::cout << "Skipping: " << q.tostring(_class_decoder) << std::endl;
				}
			}
		}

		if (sample % 10 == 9) {
			cerr << " [LLH=" << lm.log_likelihood() << "]" << endl;
			if (sample % 30u == 29)
				lm.resample_hyperparameters(eng);
		} else {
			std::cerr << "(" << decrements << ")" << flush;
			//cerr << '.' << flush;
		}
	}

	std::cout << "Done for now" << std::endl;
	exit(4);

	// TESTING

	boost::filesystem::path test_dir(test_input_directory);
	boost::filesystem::directory_iterator test_bit(test_dir), test_beod;

	std::vector<std::string> test_input_files;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(test_bit, test_beod)){
	if(is_regular_file(p) && p.extension() == ".txt")
	{
		test_input_files.push_back(p.string());
	}
}

	std::cout << "Found " << test_input_files.size() << " files" << std::endl;

//	_class_encoder.build(train_input_files, true);
//	_class_encoder.save("/tmp/tmpout/cpyp.colibri.cls");

	std::string test_dat_output_file = "/tmp/tmpout/cpyp.test.colibri.dat";

	for (auto i : train_input_files) {
		_class_encoder.encodefile(i, test_dat_output_file, false, false, true, true);
	}

	_class_decoder.load("/tmp/tmpout/cpyp.colibri.cls");

	IndexedCorpus _test_indexed_corpus = IndexedCorpus(test_dat_output_file);

	PatternModel<uint32_t> _test_pattern_model = PatternModel<uint32_t>(&_test_indexed_corpus);
	_test_pattern_model.train(test_dat_output_file, _pattern_model_options, nullptr);

	std::cout << ">> maxn:" << _test_pattern_model.maxlength() << std::endl;

	cerr << "Reading corpus...\n";
	cerr << "E-corpus size: " << _test_indexed_corpus.sentences() << " sentences\t (" << _test_pattern_model.types() << " word types (bugged), " << _test_pattern_model.size()
			<< " patterns types and " << _test_pattern_model.tokens() << " word tokens)\n";

	/*

	 vector<vector<unsigned> > test;
	 ReadFromFile(test_file, &dict, &test, &tv);


	 double llh = 0;
	 unsigned cnt = 0;
	 unsigned oovs = 0;
	 for (auto& s : test)
	 {
	 //                ctx.resize(kORDER - 1);
	 //                for (unsigned i = 0; i <= s.size(); ++i)
	 //                {
	 //                        unsigned w = (i < s.size() ? s[i] : kEOS);
	 //                        double lp = log(lm.prob(w, ctx)) / log(2);
	 //                        if (i < s.size() && vocabe.count(w) == 0)
	 //                        {
	 //                                cerr << "**OOV ";
	 //                                ++oovs;
	 //                                lp = 0;
	 //                        }
	 //                        cerr << "p(" << dict.Convert(w) << " |";
	 //                        for (unsigned j = ctx.size() + 1 - kORDER; j < ctx.size(); ++j)
	 //                                cerr << ' ' << dict.Convert(ctx[j]);
	 //                        cerr << ") = " << lp << endl;
	 //                        ctx.push_back(w);
	 //                        llh -= lp;
	 //                        cnt++;
	 //                }
	 }
	 cnt -= oovs;
	 cerr << "  Log_10 prob: " << (-llh * log(2) / log(10)) << endl;
	 cerr << "        Count: " << cnt << endl;
	 cerr << "         OOVs: " << oovs << endl;
	 cerr << "Cross-entropy: " << (llh / cnt) << endl;
	 cerr << "   Perplexity: " << pow(2, llh / cnt) << endl;
	 return 0;
	 */
}

