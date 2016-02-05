#ifndef HPYPLM_H_
#define HPYPLM_H_

#include <vector>
#include <unordered_map>
#include <map>
#include <set>

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

std::vector<PatternPointer> generateSkips(const PatternPointer& p) {
    std::vector<PatternPointer> skip_patterns = std::vector<PatternPointer>();

    if(p.size() >= 1) {
        for(int i = 1; i < p.size(); ++i) {
            PatternPointer q = p.addskip(std::pair<int, int>(i,1));
            if(q!=p) {
                skip_patterns.push_back(q);
            }
        }
    }

    return skip_patterns;
}

// represents an N-gram LM
template<unsigned N> struct PYPLM {
	PYPLM() :
			backoff(0, 1, 1, 1, 1), tr(1, 1, 1, 1, 0.8, 0.0) {
	}
	explicit PYPLM(unsigned vs, double da = 1.0, double db = 1.0, double ss = 1.0, double sr = 1.0) :
			backoff(vs, da, db, ss, sr), tr(da, db, ss, sr, 0.8, 0.0) {
	}
	template<typename Engine>
	void increment(const PatternPointer& w, const PatternPointer& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
                std::cout << "I" << N << " -------------" << std::endl;
                std::cout << "  [" << context.tostring(*decoder) << "," << w.tostring(*decoder) << "]" << std::endl;
                Pattern abc = context.pattern();
                std::cout << ":  " << abc.tostring(*decoder) << std::endl;
                abc = abc.reverse();
                std::cout << ":: " << abc.tostring(*decoder) << std::endl;
                PatternPointer rev = context.pattern().reverse();
                PatternPointer lookup = PatternPointer(rev, 0, N-1);
                std::cout << "  -- [" << rev.tostring(*decoder) << ";" << lookup.tostring(*decoder) << "]" << std::endl;

		const double bo = backoff.prob(w, context, decoder, false);

		auto it = p.find(lookup);
		if (it == p.end()) {
			it = p.insert(make_pair(lookup, crp<PatternPointer>(0.8, 0))).first;
			tr.insert(&it->second); // add to resampler
		}

		if (it->second.increment(w, bo, eng, decoder)) {
			backoff.increment(w, context, eng, decoder);
		}

	}

	template<typename Engine>
	void decrement(const PatternPointer& w, const PatternPointer& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
                PatternPointer rev = context.pattern().reverse();
                PatternPointer pattern = PatternPointer(rev, 0, N-1);
                PatternPointer shortened_context = pattern.pattern().reverse();

		auto it = p.find(pattern);
		assert(it != p.end());

		if (it->second.decrement(w, eng)) {
			backoff.decrement(w, context, eng, decoder);
		}
	}

	double prob(const PatternPointer& w, const PatternPointer& context, ClassDecoder * const decoder = nullptr, bool backoff_to_skips = false) const {
                std::cout << "P" << N << " -------------" << std::endl;

                PatternPointer rev = context.pattern().reverse();
                PatternPointer lookup = PatternPointer(rev, 0, N-1);
                std::cout << "\t(" << N << ") From:" << rev.tostring(*decoder) << " to:" << lookup.tostring(*decoder) << std::endl;
                //PatternPointer r = (N==1) ? PatternPointer() : PatternPointer(context.pattern().reverse(), 0, N-1); 
                
		const double bo = backoff.prob(w, context, decoder, backoff_to_skips);
		auto it = p.find(lookup);
		if (it == p.end()) { // if the pattern is not in the train data
                    return bo;
                }

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
		//ar & tr;
                ar & p;
	}

	PYPLM<N - 1> backoff;
	tied_parameter_resampler<crp<PatternPointer>> tr;
	std::unordered_map<PatternPointer, crp<PatternPointer>> p;  // .first = context .second = CRP
};

}

#endif
