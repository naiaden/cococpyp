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

std::vector<Pattern> generateSkips(const Pattern& p) {
    std::vector<Pattern> skip_patterns = std::vector<Pattern>();

    if(p.size() > 1) 
    {
        for(int i = 1; i < p.size(); ++i) 
        {
            Pattern q = p.addskip(std::pair<int, int>(i,1));
            if(q!=p) 
            {
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
	void increment(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
		const double bo = backoff.prob(w, context, decoder);

                Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1); 

		auto it = p.find(lookup);
		if (it == p.end()) {
			it = p.insert(make_pair(lookup, crp<Pattern>(0.8, 0))).first;
			tr.insert(&it->second); // add to resampler
		}

		if (it->second.increment(w, bo, eng)) {
			backoff.increment(w, context, eng, decoder);
		}
	}

	template<typename Engine>
	void decrement(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
                Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1); 

		auto it = p.find(lookup);
		assert(it != p.end());

		if (it->second.decrement(w, eng)) {
			backoff.decrement(w, context, eng, decoder);
		}
	}


	double probFull(const Pattern& w, const Pattern& context, SNCBWCoCoInitialiser * const cci = nullptr) const
	{
		Pattern pContext = (N==1) ? Pattern() : Pattern(context, kORDER-N, N-1);
		std::vector<Pattern> sPatterns;
		if(N!=kORDER)
			sPatterns = generateSkips(context);
		if(N==1 && context.size()==1)
		{
			sPatterns.push_back(context);
		} else
		{
			sPatterns.push_back(pContext);
		}

		std::vector<double> sPatternProbs;
		for(const Pattern& pattern : sPatterns)
		{
			double bla = backoff.probFull(w, pattern, cci);

			Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);
			auto it = p.find(lookup);
			if(it != p.end())
			{
				sPatternProbs.push_back(it->second.prob(w, bla));
			} else
			{
				sPatternProbs.push_back(bla);
			}
		}

		std::vector<double> sPatternWeights;
		double sPatternWeightSum = 0.0;
		for(const Pattern& pattern : sPatterns)
		{
			sPatternWeights.push_back(1.0);
			sPatternWeightSum += 1.0;
		}

		double probSum = 0.0;
		for(int i = 0; i < sPatterns.size(); ++i)
		{
			probSum += (sPatternWeights[i] * sPatternProbs[i]);
		}

		return probSum/sPatternWeightSum;
	}

	double probLimited(const Pattern& w, const Pattern& context, ContextCounts* contextCounts, SNCBWCoCoInitialiser * const cci = nullptr) const
	{
		Pattern pContext = (N==1) ? Pattern() : Pattern(context, kORDER-N, N-1);
		std::vector<Pattern> sPatterns;
		if(N!=kORDER)
			sPatterns = generateSkips(context);
		if(N==1 && context.size()==1)
		{
			sPatterns.push_back(context);
		} else
		{
			sPatterns.push_back(pContext);
		}

		std::vector<double> sPatternProbs;
		for(const Pattern& pattern : sPatterns)
		{
			double bla = backoff.probLimited(w, pattern, contextCounts, cci);

			Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);
			auto it = p.find(lookup);
			if(it != p.end())
			{
				const long int invDelta = contextCounts->V - contextCounts->get(lookup.reverse());

//				bool backoff = cci->trainPatternModel.frequency(pattern + w) > 0 ? false : true;

				sPatternProbs.push_back(it->second.probLimited(w, bla, invDelta));
			} else
			{
				sPatternProbs.push_back(bla);
			}
		}

		std::vector<double> sPatternWeights;
		double sPatternWeightSum = 0.0;
		for(const Pattern& pattern : sPatterns)
		{
			sPatternWeights.push_back(1.0);
			sPatternWeightSum += 1.0;
		}

		double probSum = 0.0;
		for(int i = 0; i < sPatterns.size(); ++i)
		{
			probSum += (sPatternWeights[i] * sPatternProbs[i]);
		}

		return probSum/sPatternWeightSum;
	}

	double prob(const Pattern& w, const Pattern& context, SNCBWCoCoInitialiser * const cci = nullptr) const {

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
