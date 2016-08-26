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


	double probFull(const Pattern& w, const Pattern& context,
				ContextCounts* contextCounts, ContextValues* contextValues,
				CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
		{
			bool debug = true;

//			Pattern pContext = (N==1) ? Pattern() : Pattern(context, kORDER-N, N-1);
			Pattern pContext = (N==1) ? Pattern() : Pattern(context, kORDER-N, N-1);

//			std::cout << indent << "[" << N << "] w: " << w.tostring(cci->classDecoder) << std::endl;
//			std::cout << indent << "[" << N << "] context: " << context.tostring(cci->classDecoder) << std::endl;
//			std::cout << indent << "[" << N << "] pContext: " << pContext.tostring(cci->classDecoder) << std::endl;




			std::vector<Pattern> sPatterns;



			if(N == kORDER)
			{
				sPatterns.push_back(context);
			} else
			{
				sPatterns = generateSkips(context);
				sPatterns.push_back(pContext);
			}

//			if(N!=kORDER)
//			{
//				sPatterns = generateSkips(context, &(cci->classDecoder));
//				sPatterns = generateSkips(context);
//			}
//			if(N==1 && context.size()==1)
//			{
//				sPatterns.push_back(context);
//				std::cout << indent << "[" << N << "] context: " << context.tostring(cci->classDecoder) << std::endl;
//			} else
//			{
//				sPatterns.push_back(pContext);
//				std::cout << indent << "[" << N << "] pContext: " << pContext.tostring(cci->classDecoder) << std::endl;
//			}

//			sPatterns.push_back(pContext);
//			std::cout << indent << "[" << N << "] pContext: " << pContext.tostring(cci->classDecoder) << std::endl;
//			sPatterns.push_back(context);
//			std::cout << indent << "[" << N << "] context: " << context.tostring(cci->classDecoder) << std::endl;





			std::vector<double> sPatternProbs;
			std::vector<double> sPatternWeights;
			double sPatternWeightSum = 0.0;
			double probSum = 0.0;

			for(const Pattern& sPattern : sPatterns)
			{



				double weight = contextValues->get(sPattern, w, cci, indent);
				sPatternWeights.push_back(weight);
				sPatternWeightSum += weight;



				double bla = backoff.probFull(w, sPattern, contextCounts, contextValues, cci, indent + "\t");

				std::cout << indent << "[" << N << "] context: " << sPattern.tostring(cci->classDecoder) << std::endl;

				Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);
				lookup = lookup.reverse();

//				std::cout << indent << "[" << N << "]\t Looking for " << lookup.tostring(cci->classDecoder) << std::endl;

				double probability = 0.0;
				auto it = p.find(lookup);
				if(it != p.end())
				{
//					const long int invDelta = contextCounts->V - contextCounts->get(lookup.reverse());
					const long int invDelta = 1;//contextCounts->V - contextCounts->get(lookup); // MOET DIT GEEN CONSTANTE ZIJN?
					double boob = it->second.probLimited(w, bla, invDelta);
					probability = boob;
					if(debug)
					{
						std::cout << indent << "[" << N << "]\t Looking for \"" << lookup.tostring(cci->classDecoder) << "\"" << std::endl;
						std::cout << indent << "[" << N << "]\t BOOB " << boob << " with weight: " << weight << " and delta: " << contextCounts->get(lookup) << " and p0/bla: " << bla << std::endl;
					}
				} else
				{
					probability = bla;
					if(debug)
					{
						std::cout << indent << "[" << N << "]\t Looking for \"" << lookup.tostring(cci->classDecoder) << "\"" << std::endl;
						std::cout << indent << "[" << N << "]\t BLA " << bla << " with weight: " << weight << " and delta: " << contextCounts->get(lookup) << " and p0/bla: " << bla << std::endl;
					}
				}

				sPatternProbs.push_back(probability);

				probSum += (weight * probability);
			}

			return probSum/sPatternWeightSum;

		}

	double probLimited(const Pattern& w, const Pattern& context, PatternCounts* patternCounts,
					ContextCounts* contextCounts, ContextValues* contextValues,
					CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
			{
				bool debug = false;

				Pattern pContext = (N==1) ? Pattern() : Pattern(context, kORDER-N, N-1);

				std::vector<Pattern> sPatterns;

				if(N == kORDER)
				{
					sPatterns.push_back(context);
				} else
				{
					sPatterns = generateSkips(context);
					sPatterns.push_back(pContext);
				}

				std::vector<double> sPatternProbs;
				std::vector<double> sPatternWeights;
				double sPatternWeightSum = 0.0;
				double probSum = 0.0;

				for(const Pattern& sPattern : sPatterns)
				{
					bool recursive = patternCounts->get(sPattern + w) > 0 ? false : true;

					double bla = 0.5; // ??
					if(recursive)
					{
						bla = backoff.probLimited(w, sPattern, patternCounts, contextCounts, contextValues, cci, indent + "\t");
					}


					double weight = contextValues->get(sPattern, w, cci, indent);
					sPatternWeights.push_back(weight);
					sPatternWeightSum += weight;



					Pattern lookup = (N==1) ? Pattern() : Pattern(context.reverse(), 0, N-1);
					lookup = lookup.reverse();

					double probability = 0.0;
					auto it = p.find(lookup);
					if(it != p.end())
					{
						const long int invDelta = contextCounts->V - contextCounts->get(lookup);
						double boob = it->second.probLimited(w, bla, invDelta);
						probability = boob;
						if(debug)
						{
							std::cout << indent << "[" << N << "]\t Looking for \"" << lookup.tostring(cci->classDecoder) << "\"" << std::endl;
							std::cout << indent << "[" << N << "]\t BOOB " << boob << " with weight: " << weight << " and delta: " << contextCounts->get(lookup) << std::endl;
						}
					} else
					{
						probability = bla;
						if(debug)
						{
							std::cout << indent << "[" << N << "]\t Looking for \"" << lookup.tostring(cci->classDecoder) << "\"" << std::endl;
							std::cout << indent << "[" << N << "]\t BLA " << bla << " with weight: " << weight << " and delta: " << contextCounts->get(lookup) << std::endl;
						}
					}

					sPatternProbs.push_back(probability);

					probSum += (weight * probability);
				}

				return probSum/sPatternWeightSum;

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
