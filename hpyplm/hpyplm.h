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
	void increment(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder) {
		const double bo = backoff.prob(w, context);

		Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), N-1);
//		Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), context.size() - 1);
		pattern = pattern.reverse();



		//if(decoder != nullptr) {
		std::string cs = context.tostring(*decoder);
		std::string fs = w.tostring(*decoder);
		std::string ps = pattern.tostring(*decoder);

		auto it = p.find(pattern);
		if (it == p.end()) {
			it = p.insert(make_pair(pattern, crp<Pattern>(0.8, 0))).first;
			tr.insert(&it->second); // add to resampler
		}

		if (fs == ":" && cs == "o.a. uit") {
			std::cout << "[" << cs << "/" << ps << "] +++    " << it->second.num_customers(w) << std::endl;
		}

		if (it->second.increment(w, bo, eng, false/*cs == "o.a. uit" && fs == ":"*/)) {
			backoff.increment(w, context, eng, decoder);
		}

		//if (cs == "o.a. uit" && fs == ":") {
		//
		//    do {
		//           std::cout << '\n' <<"Press the Enter key to continue.";
		//          } while (std::cin.get() != '\n');

		//}

	}

	template<typename Engine>
	void decrement(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder) {
		Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), N-1);
//		Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), context.size() - 1);
		pattern = pattern.reverse();

		auto it = p.find(pattern);
		assert(it != p.end());
		//if(decoder != nullptr) std::cout << "de(" << N << ")--\t\tf:[" << w.tostring(*decoder) << "] c:[" << context.tostring(*decoder) << "]" << std::endl;

		std::string fs = "";
		std::string cs = context.tostring(*decoder);
		if (decoder != nullptr) {
			fs = w.tostring(*decoder);
		}

		if (fs == ":" && cs == "o.a. uit") {
			std::cout << "   --- " << it->second.num_customers(w) << std::endl;
		}

		if (it->second.decrement(w, eng)) {
			if (decoder != nullptr)
				std::cout << "X" << std::endl;
			//if(decoder != nullptr) std::cout << "bo(" << N << ")--\t\tc:[" << w.tostring(*decoder) << "] c:[" << context.tostring(*decoder) << "]" << std::cout;
			backoff.decrement(w, context, eng, decoder);
			//backoff.decrement(w, context, eng, nullptr);
		}
	}

	double prob(const Pattern& w, const Pattern& context) const {
		const double bo = backoff.prob(w, context);

		Pattern pattern = Pattern(context, context.size() - 1 - (N - 1), context.size() - 1);
		pattern.reverse();

		auto it = p.find(pattern);
		if (it == p.end())
			return bo;
		return it->second.prob(w, bo);
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

	PYPLM<N - 1> backoff;
	tied_parameter_resampler<crp<Pattern>> tr;
	std::unordered_map<const Pattern, crp<Pattern>> p;  // .first = context .second = CRP
};

}

#endif
