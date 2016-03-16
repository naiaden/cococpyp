#ifndef _UNIFORM_VOCAB_H_
#define _UNIFORM_VOCAB_H_

#include <cassert>
#include <vector>

#include <pattern.h>
#include <classdecoder.h>

#include "ContextValues.h"

namespace cpyp {

// uniform distribution over a fixed vocabulary
struct UniformVocabulary {
  UniformVocabulary(unsigned vs, double, double, double, double) : p0(1.0 / vs), draws() {/*std::cerr << "[vs:" << vs << ",p0:" << p0 << "]";*/}
  template<typename Engine>
  void increment(const Pattern&, const Pattern&, Engine&, const ClassDecoder *const) { ++draws; }
  template<typename Engine>
  void decrement(const Pattern&, const Pattern&, Engine&, const ClassDecoder *const) { --draws; assert(draws >= 0); }
  double prob(const Pattern& , const Pattern&, SNCBWCoCoInitialiser * const cci) const { return p0; }
  double probLimited(const Pattern& , const Pattern&, ContextCounts*, SNCBWCoCoInitialiser * const cci) const { return p0; }
  double probFull(const Pattern& , const Pattern&, ContextCounts*, SNCBWCoCoInitialiser * const cci) const { return p0; }
  double glm_prob(const Pattern& , const Pattern&, const ClassDecoder *const) const { /*std::cout << "\t\t\tp(0) " << p0 << std::endl; */return p0; }
  template<typename Engine>
  void resample_hyperparameters(Engine&) {}
  double log_likelihood() const { /*std::cerr << "(draws:" << draws << ",log(p0):" << log(p0) << ")";*/ return draws * log(p0); }
  template<class Archive> void serialize(Archive& ar, const unsigned int) {
    ar & p0;
    ar & draws;
  }
  double p0;
  int draws;


  double j0(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j1(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j2(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j3(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j4(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j5(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j6(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j7(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j8(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j9(const Pattern&,  const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j10(const Pattern&, const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j11(const Pattern&, const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j12(const Pattern&, const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j13(const Pattern&, const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j14(const Pattern&, const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
  double j15(const Pattern&, const Pattern&, const ClassDecoder *const) { std::cerr << "SHOULDN'T BE HERE!" << std::endl; }
};

}

#endif
