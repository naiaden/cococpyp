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

#include "ContextValues.h"
#include "ContextCounts.h"

#include "PLNCache.h"

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

std::vector<Pattern> generateSkips(const Pattern& p, ClassDecoder* classDecoder = nullptr) {
    std::vector<Pattern> skip_patterns = std::vector<Pattern>();

    if(classDecoder)
    {
    	std::cout << "Generating skips for " << p.tostring(*classDecoder) << std::endl;
    }

    if(p.size() > 1) 
    {
        for(int i = 1; i < p.size(); ++i) 
        {
            Pattern q = p.addskip(std::pair<int, int>(i,1));

            if(q!=p)
            {
            	if(classDecoder)
				{
					std::cout << "\t->" << q.tostring(*classDecoder) << std::endl;
					if(q.isgap(0))
						std::cout << "ASDASDASDASD GAAAAP" << std::endl;
				}

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
	void increment(const Pattern& w, const Pattern& context, Engine& eng, CoCoInitialiser * const cci = nullptr) {
		const double bo = backoff.prob(w, context, cci);

                Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1); 

		auto it = p.find(lookup);
		if (it == p.end()) {
			it = p.insert(make_pair(lookup, crp<Pattern>(0.8, 0))).first;
			tr.insert(&it->second); // add to resampler
		}

		if (it->second.increment(w, bo, eng)) {
			backoff.increment(w, context, eng, cci);
		}
	}

	template<typename Engine>
	void decrement(const Pattern& w, const Pattern& context, Engine& eng, CoCoInitialiser * const cci = nullptr) {
                Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1); 

		auto it = p.find(lookup);
		assert(it != p.end());

		if (it->second.decrement(w, eng)) {
			backoff.decrement(w, context, eng, cci);
		}
	}

	double limitedNaiveOccurs(const Pattern& pattern, PatternCounts* _patternCounts) const
	{
		return _patternCounts->get(pattern) > 0 ? true : false;
	}

	double probLimitedNaiveHelper(const Pattern& w, const Pattern& context, const Pattern& pattern, double p0, double S, ContextCounts* contextCounts, LimitedInformation* limitedInformation, CoCoInitialiser * const cci = nullptr) const
	{
		if(pattern.size() != N)
		{
			if(cci) std::cout << "My level is " << N << " but the pattern has length " << pattern.size() << std::endl;
			return backoff.probFullNaiveHelper(w, context, pattern, p0, cci);
		}

		if(cci) std::cout << "Doing something with co" << N << "gram " << pattern.tostring(cci->classDecoder) << std::endl;
		auto it = p.find(context.reverse());
		if(it == p.end())
		{
			return p0;
		} else
		{
			return it->second.probNaive(context, w, limitedInformation, p0, S);

		}

	}



		double probLimitedNaive(const Pattern& w, const Pattern& context,PatternCounts* _patternCounts,
						ContextCounts* contextCounts, ContextValues* contextValues, LimitedCounts * limitedCounts,
						CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
		{
			bool debug = true;

			PLNCache plnCache(w, context, _patternCounts);

			Pattern pattern(context+w);

			if(pattern.size() != 4)
			{
				std::cerr << "Do something: Pattern length is not 4" << std::endl;
			}

			CoCoInitialiser* temp_cciPtr = nullptr;

			return 5.0;

		}


	double probFullNaiveHelper(const Pattern& w, const Pattern& context, const Pattern& pattern, double p0, CoCoInitialiser * const cci = nullptr) const
	{
		if(pattern.size() != N)
		{
			if(cci) std::cout << "My level is " << N << " but the pattern has length " << pattern.size() << std::endl;
			return backoff.probFullNaiveHelper(w, context, pattern, p0, cci);
		}

		if(N == 1)
		{
			if(cci) std::cout << "Doing something with unigram" << pattern.tostring(cci->classDecoder) << std::endl;
			auto it = p.find(Pattern());
			if (it == p.end()) { // if the pattern is not in the train data
				if(cci) std::cout << "It's not in the training data, but whatever" << std::endl;;
				return p0;
			}

			return it->second.prob(w, p0);
		}


		if(cci) std::cout << "Doing something with co" << N << "gram " << pattern.tostring(cci->classDecoder) << std::endl;
		auto it = p.find(context);
		if (it == p.end()) { // if the pattern is not in the train data
			return p0;
		}

		return it->second.prob(w, p0);


	}

	double probFullNaive(const Pattern& w, const Pattern& context,
					ContextCounts* contextCounts, ContextValues* contextValues,
					CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
	{
		bool debug = false;

		Pattern pattern = context + w;
		if(pattern.size() != 4)
		{
			std::cerr << "Do something: Pattern length is not 4" << std::endl;
		}

		// 4 content words
		// a b c d
		Pattern abcd = pattern;

		// 3 content words
		// a b   d
		Pattern abxd = abcd.addskip(std::pair<int, int>(1,1));
		// a   c d
		Pattern axcd = abcd.addskip(std::pair<int, int>(2,1));
		//   b c d
		Pattern xbcd = Pattern(abcd,1,3);

		// 2 content words
		// a     d
		Pattern axxd = abcd.addskip(std::pair<int, int>(1,2));
		//   b   d
		Pattern xbxd = xbcd.addskip(std::pair<int, int>(1,1));
		//     c d
		Pattern xxcd = Pattern(xbcd,1,2);

		// 1 content word
		//       d
		Pattern xxxd = Pattern(xxcd,1,1);

		// 0 content words
		//
		Pattern xxxx = Pattern();

		CoCoInitialiser* temp_cciPtr = nullptr;

		// -----------------------------
		// 0
		double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
		if(debug) std::cout << "xxxx p: " << xxxx_prob << std::endl;

		// 1
		double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
		if(debug) std::cout << "xxxd p: " << xxxd_prob << std::endl;

		// 2
		double axxd_prob = probFullNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, xxxd_prob, temp_cciPtr);
		double axxd_weight = contextValues->get(axxd);
		if(debug) std::cout << "axxd p: " << axxd_prob << " with weight: " << axxd_weight << std::endl;
		double xbxd_prob = probFullNaiveHelper(xxxd, Pattern(xbxd, 0, 2), xbxd, xxxd_prob, temp_cciPtr);
		double xbxd_weight = contextValues->get(xbxd);
		if(debug) std::cout << "xbxd p: " << xbxd_prob << " with weight: " << xbxd_weight << std::endl;
		double xxcd_prob = probFullNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxxd_prob, temp_cciPtr);
		double xxcd_weight = contextValues->get(xxcd);
		if(debug) std::cout << "xxcd p: " << xxcd_prob << " with weight: " << xxcd_weight << std::endl;

		double c2_prob = (axxd_prob * axxd_weight + xbxd_prob * xbxd_weight + xxcd_prob * xxcd_weight) / (axxd_weight + xbxd_weight + xxcd_weight);

		// 3
		double abxd_prob = probFullNaiveHelper(xxxd, Pattern(abxd, 0, 3), abxd, c2_prob, temp_cciPtr);
		double abxd_weight = contextValues->get(abxd);
		if(debug) std::cout << "abxd p: " << abxd_prob << " with weight: " << abxd_weight << std::endl;
		double axcd_prob = probFullNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, c2_prob, temp_cciPtr);
		double axcd_weight = contextValues->get(axcd);
		if(debug) std::cout << "axcd p: " << axcd_prob << " with weight: " << axcd_weight << std::endl;
		double xbcd_prob = probFullNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, c2_prob, temp_cciPtr);
		double xbcd_weight = contextValues->get(xbcd);
		if(debug) std::cout << "xbcd p: " << xbcd_prob << " with weight: " << xbcd_weight << std::endl;

		double c3_prob = (abxd_prob * abxd_weight + axcd_prob * axcd_weight + xbcd_prob * xbcd_weight) / (abxd_weight + axcd_weight + xbcd_weight);

		// 4
		double abcd_prob = probFullNaiveHelper(xxxd, Pattern(abcd, 0, 3), abcd, c3_prob, temp_cciPtr);
		double abcd_weight = 1.0;
		if(debug) std::cout << "abcd p: " << abcd_prob << " with weight: " << abcd_weight << std::endl;

		return abcd_prob;

	}

	double probFullNaive1(const Pattern& w, const Pattern& context,
							ContextCounts* contextCounts, ContextValues* contextValues,
							CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
			{
				bool debug = false;

				Pattern pattern = context + w;
				if(pattern.size() != 1)
				{
					std::cerr << "[ProbFullNaive1]\tDo something: Pattern length is not 1" << std::endl;
				}

				// 1 word
				Pattern xxxd = pattern;
				// 0 words
				Pattern xxxx = Pattern();

				CoCoInitialiser* temp_cciPtr = nullptr;

				// -----------------------------
				// 0
				double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
				if(debug) std::cout << "xxxx p: " << xxxx_prob << std::endl;

				// 1
				double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
				if(debug) std::cout << "xxxd p: " << xxxd_prob << std::endl;

				return xxxd_prob;
			}

	double probFullNaive2(const Pattern& w, const Pattern& context,
						ContextCounts* contextCounts, ContextValues* contextValues,
						CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
		{
			bool debug = false;

			Pattern pattern = context + w;
			if(pattern.size() != 2)
			{
				std::cerr << "[ProbFullNaive2]\tDo something: Pattern length is not 2" << std::endl;
			}

			// 2 words
			Pattern xxcd = pattern;
			// 1 word
			Pattern xxxd = Pattern(xxcd,1,1);
			// 0 words
			Pattern xxxx = Pattern();

			CoCoInitialiser* temp_cciPtr = nullptr;

			// -----------------------------
			// 0
			double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
			if(debug) std::cout << "xxxx p: " << xxxx_prob << std::endl;

			// 1
			double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
			if(debug) std::cout << "xxxd p: " << xxxd_prob << std::endl;

			// 2
			double xxcd_prob = probFullNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxxd_prob, temp_cciPtr);
			double xxcd_weight = contextValues->get(xxcd);
			if(debug) std::cout << "xxcd p: " << xxcd_prob << " with weight: " << xxcd_weight << std::endl;

			double c2_prob = xxcd_prob;// * xxcd_weight / xxcd_weight;
			return c2_prob;
		}

	double probFullNaive3(const Pattern& w, const Pattern& context,
						ContextCounts* contextCounts, ContextValues* contextValues,
						CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
		{
			bool debug = false;

			Pattern pattern = context + w;
			if(pattern.size() != 3)
			{
				std::cerr << "[ProbFullNaive3]\tDo something: Pattern length is not 3" << std::endl;
			}


			// 3 words
			Pattern xbcd = pattern;
			// 2 words
			Pattern xbxd = xbcd.addskip(std::pair<int, int>(1,1));
			Pattern xxcd = Pattern(xbcd,1,2);
			// 1 word
			Pattern xxxd = Pattern(xxcd,1,1);
			// 0 words
			Pattern xxxx = Pattern();

			CoCoInitialiser* temp_cciPtr = nullptr;

			// -----------------------------
			// 0
			double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
			if(debug) std::cout << "xxxx p: " << xxxx_prob << std::endl;

			// 1
			double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
			if(debug) std::cout << "xxxd p: " << xxxd_prob << std::endl;

			// 2
			double xbxd_prob = probFullNaiveHelper(xxxd, Pattern(xbxd, 0, 2), xbxd, xxxd_prob, temp_cciPtr);
			double xbxd_weight = contextValues->get(xbxd);
			if(debug) std::cout << "xbxd p: " << xbxd_prob << " with weight: " << xbxd_weight << std::endl;
			double xxcd_prob = probFullNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxxd_prob, temp_cciPtr);
			double xxcd_weight = contextValues->get(xxcd);
			if(debug) std::cout << "xxcd p: " << xxcd_prob << " with weight: " << xxcd_weight << std::endl;

			double c2_prob = (xbxd_prob * xbxd_weight + xxcd_prob * xxcd_weight) / (xbxd_weight + xxcd_weight);

			// 3
			double xbcd_prob = probFullNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, c2_prob, temp_cciPtr);
			double xbcd_weight = contextValues->get(xbcd);
			if(debug) std::cout << "xbcd p: " << xbcd_prob << " with weight: " << xbcd_weight << std::endl;

			double c3_prob = xbcd_prob;// * xbcd_weight / xbcd_weight;
			return c3_prob;

		}

	double prob(const Pattern& w, const Pattern& context, CoCoInitialiser * const cci = nullptr) const {

		const double bo = backoff.prob(w, context, cci);

                Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1); 

//                std::cout << "\t" << N << " LOOKUP: " << lookup.tostring(*decoder) << std::endl;

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
	tied_parameter_resampler<crp<Pattern>> tr;
	std::unordered_map<Pattern, crp<Pattern>> p;  // .first = context .second = CRP
};

}

#endif
