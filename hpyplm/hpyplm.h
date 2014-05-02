#ifndef HPYPLM_H_
#define HPYPLM_H_

#include <vector>
#include <unordered_map>

#include "cpyp/m.h"
#include "cpyp/random.h"
#include "cpyp/crp.h"
#include "cpyp/tied_parameter_resampler.h"

#include "hpyplm/uvector.h"
#include "hpyplm/uniform_vocab.h"

#include <pattern.h>

// A not very memory-efficient implementation of an N-gram LM based on PYPs
// as described in Y.-W. Teh. (2006) A Hierarchical Bayesian Language Model
// based on Pitman-Yor Processes. In Proc. ACL.

namespace cpyp {

template <unsigned N> struct PYPLM;

template<> struct PYPLM<0> : public UniformVocabulary {
  PYPLM(unsigned vs, double a, double b, double c, double d) :
    UniformVocabulary(vs, a, b, c, d) {}
};

// represents an N-gram LM
template <unsigned N> struct PYPLM {
  PYPLM() :
      backoff(0,1,1,1,1),
      tr(1,1,1,1,0.8,0.0) {}
  explicit PYPLM(unsigned vs, double da = 1.0, double db = 1.0, double ss = 1.0, double sr = 1.0) :
      backoff(vs, da, db, ss, sr),
      tr(da, db, ss, sr, 0.8, 0.0) {}
  template<typename Engine>
  void increment(Pattern w, Pattern context, Engine& eng) {
    const double bo = backoff.prob(w, context);

    Pattern pattern = Pattern(context);
    pattern.reverse();

    auto it = p.find(pattern);
    if(it == p.end()) {
    	it = p.insert(make_pair(pattern, crp<Pattern>(0.8,0))).first;
    	tr.insert(&it->second); // add to resampler
    }
    if (it->second.increment(w, bo, eng))
    	backoff.increment(w, context, eng);

//    for (unsigned i = 0; i < N-1; ++i)
//      lookup[i] = context[context.size() - 1 - i]; ///// hhhhhhhhhhhhhhhhhhier
//
//
//    auto it = p.find(lookup);
//    if (it == p.end()) {
//      it = p.insert(make_pair(lookup, crp<Pattern>(0.8,0))).first;
//      tr.insert(&it->second);  // add to resampler
//    }
//    if (it->second.increment(w, bo, eng))
//      backoff.increment(w, context, eng);
  }


  template<typename Engine>
  void decrement(Pattern w, Pattern context, Engine& eng) {
	  Pattern pattern = Pattern(context);
	  pattern.reverse();

	  auto it = p.find(pattern);
	  assert(it != p.end());
	  if(it->second.decrement(w, eng))
		  backoff.decrement(w, context, eng);

//    for (unsigned i = 0; i < N-1; ++i)
//      lookup[i] = context[context.size() - 1 - i];
//    auto it = p.find(lookup);
//    assert(it != p.end());
//    if (it->second.decrement(w, eng))
//      backoff.decrement(w, context, eng);
  }
  double prob(Pattern w, Pattern context) const {
	  const double bo = backoff.prob(w, context);

	  Pattern pattern = Pattern(context);
	  pattern.reverse();

	  auto it = p.find(pattern);
	  if(it == p.end()) return bo;
	  return it->second.prob(w,bo);

//    const double bo = backoff.prob(w, context);
//    for (unsigned i = 0; i < N-1; ++i)
//      lookup[i] = context[context.size() - 1 - i];
//    auto it = p.find(lookup);
//    if (it == p.end()) return bo;
//    return it->second.prob(w, bo);
  }

  double log_likelihood() const {
    return backoff.log_likelihood() + tr.log_likelihood();
  }

  template<typename Engine>
  void resample_hyperparameters(Engine& eng) {
    tr.resample_hyperparameters(eng);
    backoff.resample_hyperparameters(eng);
  }

  template<class Archive> void serialize(Archive& ar, const unsigned int version) {
    backoff.serialize(ar, version);
    ar & p;
  }

  PYPLM<N-1> backoff;
  tied_parameter_resampler<crp<Pattern>> tr;
  std::unordered_map<Pattern, crp<Pattern>, pattern_hash> p;  // .first = context .second = CRP
};

}

#endif
