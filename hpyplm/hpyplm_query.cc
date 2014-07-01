#include <iostream>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"
#include "corpus/corpus.h"

#include "cpyp/boost_serializers.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#define kORDER 3

using namespace std;
using namespace cpyp;

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << argv[0] << " <input.lm> <test.txt>\n\nCompute perplexity of a " << kORDER << "-gram HPYP LM\n";
    return 1;
  }
  MT19937 eng;
  string lm_file = argv[1];
  string test_file = argv[2];

    ClassEncoder _class_encoder = ClassEncoder();
    ClassDecoder _class_decoder = ClassDecoder();

    boost::filesystem::path foreground_dir(_test_input_directory);
    boost::filesystem::directory_iterator fit(foreground_dir), feod;

    std::vector<std::string> test_input_files;
    BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(fit, feod)) {
        if(is_regular_file(p)) {
            test_input_files.push_back(p.string());
        }
    }

    _class_encoder.load(output_directory + "/" + "justsomemodel" + ".cls");

    for(auto i : test_input_files) {
        _class_encoder.encodefile(i, output_directory + "/" + "someothermodel" + ".dat", 1, 1, 0, 1);
    }
    _class_encoder.save(output_directory + "/" + "someothermodel" + ".cls");

    _class_decoder.load(output_directory + "/" + "someothermodel" + ".cls");

    IndexedCorpus _test_indexed_corpus = IndexedCorpus(output_directory + "/" + "someothermodel" + ".dat");

    PatternModel<uint32_t> _test_pattern_model = PatternModel<uint32_t>(&_test_indexed_corpus);
    _test_pattern_model.train(output_directory + "/" + "someothermodel" + ".dat", _test_pattern_model_options);

    // test_pattern_model_options

    _test_pattern_model.computestats();
    _test_pattern_model.computecoveragestats();

    _test_pattern_model.report(&std::cerr);


    std::cerr << ">" << _test_pattern_model.totalwordtypesingroup(0,1) << "<";



    _test_pattern_model.write(_output_directory + "/" + "someothermodel" + ".patternmodel");

    double llh = 0;
    unsigned cnt = 0;
    unsigned oovs = 0;

    for(IndexPattern indexPattern : _test_indexed_corpus) {
        for(Pattern pattern : _test_pattern_model.getreverseindex(indexPattern.ref)) {
            size_t pattern_size = pattern.size();

            Pattern context = Pattern();
            Pattern focus = Pattern();

            if(pattern_size == kORDER) {
                context = Pattern(pattern, 0, pattern_size - 1);
                focus = pattern[pattern_size - 1];

                if(pattern_size == 1) {
                    focus = pattern[0];
                } else {
                    context = Pattern(pattern, 0, pattern_size - 1);
                    focus = pattern[pattern_size - 1];
                }

                unsigned oc = _test_pattern_model.occurrencecount(focus);

                double lp = log(lm.prob(focus, context)) / log(2);
                if(!oc) {
                    ++oovs;
                    lp = 0;
                }

                std::cout << "p[" << oc << "](" << focus.tostring(_class_decoder) << " |";
                std::cout << context.tostring(_test_class_decoder) << ") = " << lp << std::endl;

                llh -= lp;
                ++cnt;
            }
        }
    }




  PYPLM<kORDER> lm;
  //vector<unsigned> ctx(kORDER - 1, kSOS);

  cerr << "Reading LM from " << lm_file << " ...\n";
  ifstream ifile(lm_file.c_str(), ios::in | ios::binary);
  if (!ifile.good()) {
    cerr << "Failed to open " << lm_file << " for reading\n";
    return 1;
  }
  boost::archive::binary_iarchive ia(ifile);
  Dict dict;
  ia & dict;
  ia & lm;
  const unsigned max_iv = dict.max();
  const unsigned kSOS = dict.Convert("<s>");
  const unsigned kEOS = dict.Convert("</s>");
  set<unsigned> tv;
  vector<vector<unsigned> > test;
  ReadFromFile(test_file, &dict, &test, &tv);
  double llh = 0;
  unsigned cnt = 0;
  unsigned oovs = 0;
  vector<unsigned> ctx(kORDER - 1, kSOS);
  for (auto& s : test) {
    ctx.resize(kORDER - 1);
    for (unsigned i = 0; i <= s.size(); ++i) {
      unsigned w = (i < s.size() ? s[i] : kEOS);
      double lp = log(lm.prob(w, ctx)) / log(2);
      if (w >= max_iv) {
        cerr << "**OOV ";
        ++oovs;
        lp = 0;
      }
      cerr << "p(" << dict.Convert(w) << " |";
      for (unsigned j = ctx.size() + 1 - kORDER; j < ctx.size(); ++j)
        cerr << ' ' << dict.Convert(ctx[j]);
      cerr << ") = " << lp << endl;
      ctx.push_back(w);
      llh -= lp;
      cnt++;
    }
  }
  cnt -= oovs;
  cerr << "  Log_10 prob: " << (-llh * log(2) / log(10)) << endl;
  cerr << "        Count: " << cnt << endl;
  cerr << "         OOVs: " << oovs << endl;
  cerr << "Cross-entropy: " << (llh / cnt) << endl;
  cerr << "   Perplexity: " << pow(2, llh / cnt) << endl;
  return 0;
}

