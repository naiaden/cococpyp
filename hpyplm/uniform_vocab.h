#ifndef _UNIFORM_VOCAB_H_
#define _UNIFORM_VOCAB_H_

#include <cassert>
#include <vector>

#include <pattern.h>
#include <classdecoder.h>

namespace cpyp {

// uniform distribution over a fixed vocabulary
struct UniformVocabulary {
  UniformVocabulary(unsigned vs, double, double, double, double) : p0(1.0 / vs), draws() {}
  template<typename Engine>
  void increment(Pattern, Pattern, Engine&, const ClassDecoder *const) { ++draws; }
  template<typename Engine>
  void decrement(Pattern, Pattern, Engine&, const ClassDecoder *const) { --draws; assert(draws >= 0); }
  double prob(Pattern, Pattern, const ClassDecoder *const) const { /*std::cout << "\t\t\tp(0) " << p0 << std::endl; */return p0; }
  template<typename Engine>
  void resample_hyperparameters(Engine&) {}
  double log_likelihood() const { return draws * log(p0); }
  template<class Archive> void serialize(Archive& ar, const unsigned int) {
    ar & p0;
    ar & draws;
  }
  double p0;
  int draws;
};

}

#endif
