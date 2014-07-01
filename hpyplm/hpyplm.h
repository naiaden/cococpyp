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

#include <classdecoder.h>

#include <pattern.h>

// A not very memory-efficient implementation of an N-gram LM based on PYPs
// as described in Y.-W. Teh. (2006) A Hierarchical Bayesian Language Model
// based on Pitman-Yor Processes. In Proc. ACL.

namespace cpyp {

template<unsigned N> struct PYPLM;

template<> struct PYPLM<0> : public UniformVocabulary {
	PYPLM(unsigned vs, double a, double b, double c, double d) :
			UniformVocabulary(vs, a, b, c, d) {
	}
};

// represents an N-gram LM
template<unsigned N> struct PYPLM {
	PYPLM() :
			backoff(0, 1, 1, 1, 1), tr(1, 1, 1, 1, 0.8, 0.0) {
	}
	explicit PYPLM(unsigned vs, double da = 1.0, double db = 1.0, double ss = 1.0, double sr = 1.0) :
			backoff(vs, da, db, ss, sr), tr(da, db, ss, sr, 0.8, 0.0) {
	}
	template<typename Engine>
	void increment(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
		const double bo = backoff.prob(w, context, decoder);

		//Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), N-1);
		//pattern = pattern.reverse();

                Pattern pattern = Pattern(context.reverse(), 0, N-1);
                //Pattern pattern = context.reverse();

                //std::cout << "\t\tI(" << N << ")[" << context.size() << "] ";
                //std::cout << pattern.tostring(*decoder) << " ";
                //std::cout << "[" << bo << "]";
                //std::cout << std::endl;

		auto it = p.find(pattern);
		if (it == p.end()) {
			it = p.insert(make_pair(pattern, crp<Pattern>(0.8, 0))).first;
			tr.insert(&it->second); // add to resampler
		}

		if (it->second.increment(w, bo, eng, false/*cs == "o.a. uit" && fs == ":"*/)) {
			backoff.increment(w, context, eng, decoder);
		}
	}

	template<typename Engine>
	void decrement(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
		//Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), N-1);
		//pattern = pattern.reverse();

                Pattern pattern = Pattern(context.reverse(), 0, N-1);


                //std::cout << "\t\tD(" << N << ")[" << context.size() << "] ";
                //std::cout << pattern.tostring(*decoder);
                //std::cout << std::endl;

		auto it = p.find(pattern);
		assert(it != p.end());

		if (it->second.decrement(w, eng)) {
			backoff.decrement(w, context, eng, decoder);
		}
	}

	double prob(const Pattern& w, const Pattern& context, ClassDecoder * const decoder = nullptr) const {
		const double bo = backoff.prob(w, context, decoder);

		//Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), N-1);
		//Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), context.size() - 1);
		//pattern = pattern.reverse();


                Pattern pattern = Pattern(context.reverse(), 0, N-1);
                //Pattern pattern = context.reverse();

		auto it = p.find(pattern);
		if (it == p.end()) {
                        //std::cout << "\t\t\tp(" << N << ")" << pattern.tostring(*decoder) << " " << bo << std::endl;
			return bo;
                        }
                //double newbo = it->second.prob(w, bo);
                //std::cout << "\t\t\tp(" << N << "] " << newbo << std::endl;
                //return newbo;
		return it->second.prob(w, bo);
	}

	double log_likelihood() const {
                //std::cerr << "[" << N << "] BO: " << backoff.log_likelihood() << " TR: " << tr.log_likelihood() << std::endl;
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

	PYPLM<N - 1> backoff;
	tied_parameter_resampler<crp<Pattern>> tr;
	std::unordered_map<const Pattern, crp<Pattern>> p;  // .first = context .second = CRP
};

}

#endif
