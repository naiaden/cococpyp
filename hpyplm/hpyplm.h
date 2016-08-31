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


		class PatternCache
		{
		private:
			bool occursChecked = false;
			bool occurs_ = false;

			bool backoffChecked = false;
			double backoff_ = 0.0;

			bool probChecked = false;
			double prob_ = 0.0;

			bool weightChecked = false;
			double weight_ = 0.0;

			bool p0Checked = false;
			double p0_ = 0.0;

			bool computed = false;

			Pattern focus;
			Pattern context;

		public:
			PatternCache() : focus(Pattern())
			{

			}

			PatternCache(const Pattern& _focus, const Pattern& _context) : focus(_focus), context(_context)
			{

			}

			bool isComputed() { return computed; }

			bool occurs(PatternCounts* _patternCounts)
			{
				if(occursChecked)
				{
					return occurs_;
				} else
				{
					occurs_ = _patternCounts->get(pattern);
					occursChecked = true;
					return occurs_;
				}
			}

			double prob()
			{
				if(probChecked)
				{
					return prob_;
				} else
				{

					probChecked = true;
					return prob_;
				}
			}
		};

		class P_XXXX : public PatternCache
		{
			P_XXXX() : PatternCache(Pattern(), Pattern()) { }
			P_XXXX(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context + focus);
					prob_ = probLimitedNaiveHelper(focus, context, context+focus, 1.0, 1.0, contextCounts, temp_cciPtr);

					computed = true;
				}
				return prob_;
			}
		};

		class P_XXXD : public PatternCache
		{
			P_XXXD() : PatternCache(Pattern(), Pattern()) { }
			P_XXXD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					if(xxxd.occurs(_patternCounts))
					{
						prob_ = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
					} else
					{
						caches.xxxx.compute();

						p0_ = caches.xxxx.getWeight() * caches.xxxx.getProb();
						backoff_ = 1.0;

						prob_ = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
					}
					computed = true;
				}
				return prob_;
			}
		};

		class P_XXCD : public PatternCache
		{
			P_XXCD() : PatternCache(Pattern(), Pattern()) { }
			P_XXCD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context+focus);
					backoff_ = 1.0;
					prob_ = 0.0;
					p0_ = 1.0;
					if(xxcd.occurs(_patternCounts))
					{

						backoff_ = 0.0;
						prob_ = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
					} else
					{
						backoff_ = 1.0;
						p0_ = caches.xxxd.getWeight() * caches.xxxd.getProb();

						prob_ = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 1, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
					}
					computed = true;
				}
				return prob_;
			}
		};

		class P_XBXD : public PatternCache
		{
			P_XBXD() : PatternCache(Pattern(), Pattern()) { }
			P_XBXD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context+focus);
					backoff_ = 1.0;
					prob_ = 0.0;
					p0_ = 1.0;
					if(xbxd.occurs(_patternCounts))
					{
						backoff_ = 0.0;
						prob_ = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 0,2), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
					} else
					{
						backoff_ = 1.0;
						p0_ = caches.xxxd.getWeight() * caches.xxxd.getProb();

						prob_ = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 1, 1), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
					}

					computed = true;
				}
				return prob_;
			}
		};

		class P_AXXD : public PatternCache
		{
			P_AXXD() : PatternCache(Pattern(), Pattern()) { }
			P_AXXD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context+focus);
					backoff_ = 1.0;
					prob_ = 0.0;
					p0_ = 1.0;
					if(axxd.occurs(_patternCounts))
					{
						backoff_ = 0.0;
						prob_ = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
					} else
					{
						backoff_ = 1.0;
						p0_ = caches.xxxd.getWeight() * caches.xxxd.getProb();

						prob_ = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
					}
					computed = true;
				}
				return prob_;
			}
		};

		class P_XBCD : public PatternCache
		{
			P_XBCD() : PatternCache(Pattern(), Pattern()) { }
			P_XBCD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context+focus);
					backoff_ = 1.0;
					prob_ = 0.0;
					p0_ = 1.0;
					if(xbcd.occurs(_patternCounts))
					{
						backoff_ = 0.0;
						prob_ = probLimitedNaiveHelper(xxxd, Pattern(xbcd, 1, 2), xbcd, xbcd_p0, xbcd_backoff, contextCounts, temp_cciPtr);
					} else
					{
						backoff_ = 1.0;
						p0_ = caches.xxcd.getWeight() * caches.xxcd.getProb() + caches.xbxd.getWeight() * caches.xbxd.getProb();

						prob_ = probLimitedNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, xbcd_p0, xbcd_backoff, contextCounts, temp_cciPtr);
					}
					computed = true;

				}
				return prob_;
		};

		class P_AXCD : public PatternCache
		{
			P_AXCD() : PatternCache(Pattern(), Pattern()) { }
			P_AXCD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context+focus);
					backoff_ = 1.0;
					prob_ = 0.0;
					p0_ = 1.0;
					if(axcd.occurs(_patternCounts))
					{
						backoff_ = 0.0;
						prob_ = probLimitedNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, axcd_p0, axcd_backoff, contextCounts, temp_cciPtr);
					} else
					{


						backoff_ = 1.0;
						p0_ = caches.axxd.getWeight() * caches.xxcd.getProb() + xxcd_weight * xxcd_prob;

						if(debug) std::cout << axcd.tostring(cci->classDecoder) << " not found" << std::endl;
						if(debug) std::cout << "--" << axxd_weight << "*" << axxd_prob << "+" << xxcd_weight << "*" << xxcd_prob << std::endl;

						axcd_prob = probLimitedNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, axcd_p0, axcd_backoff, contextCounts, temp_cciPtr);
					} // axcd

					computed = true;
				}
				return prob_;
			}
		};

		class P_ABXD : public PatternCache
		{
			P_ABXD() : PatternCache(Pattern(), Pattern()) { }
			P_ABXD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context+focus);
					backoff_ = 1.0;
					prob_ = 0.0;
					p0_ = 1.0;

					computed = true;
				}
				return prob_;
			}
		};

		class P_ABCD : public PatternCache
		{
			P_ABCD() : PatternCache(Pattern(), Pattern()) { }
			P_ABCD(const Pattern& f, const Pattern& c) : PatternCache(f,c) { }

			double compute()
			{
				if(!computed)
				{
					weight_ = contextValues->get(context+focus);
					backoff_ = 1.0;
					prob_ = 0.0;
					p0_ = 1.0;

					computed = true;
				}
				return prob_;
			}
		};


		struct PLNCache
		{
			PatternCounts* patternCounts;

			P_ABCD abcd;
			P_ABXD abxd;
			P_AXCD axcd;
			P_XBCD xbcd;
			P_AXXD axxd;
			P_XBXD xbxd;
			P_XXCD xxcd;
			P_XXXD xxxd;
			P_XXXX xxxx;

			bool debug = false;



			PLNCache(const Pattern& w, const Pattern& context, PatternCounts* pc)
			{
				abcd = P_ABCD(context, w);
				abxd = P_ABXD(context.addskip(std::pair<int, int>(2,1)), w);
				axcd = P_AXCD(context.addskip(std::pair<int, int>(1,1)), w);
				xbcd = P_XBCD(Pattern(context,1,2), w);

				axxd = P_AXXD(context.addskip(std::pair<int, int>(1,2)), w);
				xbxd = P_XBXD(Pattern(context,1,2).addskip(std::pair<int, int>(1,1)), w);
				xxcd = P_XXCD(Pattern(context,1,2), w);

				xxxd = P_XXXD(Pattern(), w);

				xxxx = P_XXXX();

				patternCounts = pc;
			}

			bool populate()
			{
				if(abcd.occurs(_patternCounts))
				{
								if(debug) std::cout << abcd.tostring(cci->classDecoder) << " found" << std::endl;
				//
				//				auto it = p.find(Pattern(abcd, 0, 3).reverse());
				//				if(it != p.end())
				//				{
				//					std::cout << "CONTEXT FOR FOUR FOUND" << std::endl;
				//
				//					// const Pattern& context, const Dish& dish, LimitedInformation* li, const F& p0 = 0.0, const double S = 1.0
				//					return it->second.probNaive(Pattern(abcd, 0, 3), xxxd,  1.0, delta, 0.0);
				//
				//				}
				} else
				{
				//
				//				double xbcd_weight = contextValues->get(xbcd);
				//				double xbcd_backoff = 1.0;
				//				double xbcd_prob = 0.0;
				//				double xbcd_p0 = 1.0;
								if(plnCache.abcd.occurs(_patternCounts))
								{
				//					xbcd_backoff = 0.0;
				//					xbcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbcd, 1, 2), xbcd, xbcd_p0, xbcd_backoff, contextCounts, temp_cciPtr);
				//
				//					if(debug) std::cout << xbcd.tostring(cci->classDecoder) << " found" << std::endl;
				//					if(debug) std::cout << "==" << xbcd_prob << std::endl;
								} else
								{
				//					double xxcd_weight = contextValues->get(xxcd);
				//					double xxcd_backoff = 1.0;
				//					double xxcd_prob = 0.0;
				//					double xxcd_p0 = 1.0;
									if(plnCache.xxcd.occurs(_patternCounts))
									{
				//
				//						xxcd_backoff = 0.0;
				//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
				//
				//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " found" << std::endl;
				//						if(debug) std::cout << "==" << xxcd_prob << std::endl;
									} else
									{
				//						double xxxd_weight = contextValues->get(xxxd);
				//						double xxxd_backoff = 1.0;
				//						double xxxd_prob = 0.0;
				//						double xxxd_p0 = 1.0;
										if(plnCache.xxxd.occurs(_patternCounts))
										{
											plnCache.xxxd.prob();
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
				//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
										} else
										{
				//							double xxxx_weight = contextValues->get(xxxx);
				//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
				//
				//							xxxd_p0 = xxxx_weight * xxxx_prob;
				//							xxxd_backoff = 1.0;
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
				//
				//
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
										} // xxxx
				//
				//
				//						xxcd_backoff = 1.0;
				//						xxcd_p0 = xxxd_weight * xxxd_prob;
				//
				//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " not found" << std::endl;
				//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
				//
				//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 1, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
				//
									} // xxcd
				//
				//					double xbxd_weight = contextValues->get(xbxd);
				//					double xbxd_backoff = 1.0;
				//					double xbxd_prob = 0.0;
				//					double xbxd_p0 = 1.0;
				//					if(limitedNaiveOccurs(xbxd, _patternCounts))
				//					{
				//						xbxd_backoff = 0.0;
				//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 0,2), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
				//
				//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " found" << std::endl;
				//						if(debug) std::cout << "==" << xbxd_prob << std::endl;
				//					} else
				//					{
				//						double xxxd_weight = contextValues->get(xxxd);
				//						double xxxd_backoff = 1.0;
				//						double xxxd_prob = 0.0;
				//						double xxxd_p0 = 1.0;
				//						if(limitedNaiveOccurs(xxxd, _patternCounts))
				//						{
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
				//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
				//						} else
				//						{
				//							double xxxx_weight = contextValues->get(xxxx);
				//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
				//
				//							xxxd_p0 = xxxx_weight * xxxx_prob;
				//							xxxd_backoff = 1.0;
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
				//
				//
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//						}
				//
				//
				//						xxcd_backoff = 1.0;
				//						xxcd_p0 = xxxd_weight * xxxd_prob;
				//
				//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
				//
				//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 1, 1), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
				//					} // xbxd
				//
				//					xbcd_backoff = 1.0;
				//					xbcd_p0 = xxcd_weight * xxcd_prob + xbxd_weight * xbxd_prob;
				//
				//					if(debug) std::cout << xbcd.tostring(cci->classDecoder) << " not found" << std::endl;
				//					if(debug) std::cout << "--" << xxcd_weight << "*" << xxcd_prob << "+" << xbxd_weight << "*" << xbxd_prob << std::endl;
				//
				//					xbcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, xbcd_p0, xbcd_backoff, contextCounts, temp_cciPtr);
								} // xbcd
				//
				//
				//
				//				double abxd_weight = contextValues->get(abxd);
				//				double abxd_backoff = 1.0;
				//				double abxd_prob = 0.0;
				//				double abxd_p0 = 1.0;
				//				if(limitedNaiveOccurs(abxd, _patternCounts))
				//				{
				//					abxd_backoff = 0.0;
				//					abxd_prob = probLimitedNaiveHelper(xxxd, Pattern(abxd, 0, 3), abxd, abxd_p0, abxd_backoff, contextCounts, temp_cciPtr);
				//
				//					if(debug) std::cout << abxd.tostring(cci->classDecoder) << " found" << std::endl;
				//					if(debug) std::cout << "==" << abxd_prob << std::endl;
				//				} else
				//				{
				//					if(debug) std::cout << xbcd.tostring(cci->classDecoder) << " not found" << std::endl;
				//					double xbxd_weight = contextValues->get(xbxd);
				//					double xbxd_backoff = 1.0;
				//					double xbxd_prob = 0.0;
				//					double xbxd_p0 = 1.0;
				//					if(limitedNaiveOccurs(xbxd, _patternCounts))
				//					{
				//						xbxd_backoff = 0.0;
				//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 0,2), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
				//
				//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " found" << std::endl;
				//						if(debug) std::cout << "==" << xbxd_prob << std::endl;
				//					} else
				//					{
				//						double xxxd_weight = contextValues->get(xxxd);
				//						double xxxd_backoff = 1.0;
				//						double xxxd_prob = 0.0;
				//						double xxxd_p0 = 1.0;
				//						if(limitedNaiveOccurs(xxxd, _patternCounts))
				//						{
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
				//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
				//						} else
				//						{
				//							double xxxx_weight = contextValues->get(xxxx);
				//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
				//
				//							xxxd_p0 = xxxx_weight * xxxx_prob;
				//							xxxd_backoff = 1.0;
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
				//
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//						}
				//
				//
				//						xbxd_backoff = 1.0;
				//						xbxd_p0 = xxxd_weight * xxxd_prob;
				//
				//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
				//
				//
				//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 0, 2), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
				//					} // xbxd
				//
				//					double axxd_weight = contextValues->get(axxd);
				//					double axxd_backoff = 1.0;
				//					double axxd_prob = 0.0;
				//					double axxd_p0 = 1.0;
				//					if(limitedNaiveOccurs(axxd, _patternCounts))
				//					{
				//						axxd_backoff = 0.0;
				//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
				//
				//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " found" << std::endl;
				//						if(debug) std::cout << "==" << axxd_prob << std::endl;
				//					} else
				//					{
				//						double xxxd_weight = contextValues->get(xxxd);
				//						double xxxd_backoff = 1.0;
				//						double xxxd_prob = 0.0;
				//						double xxxd_p0 = 1.0;
				//						if(limitedNaiveOccurs(xxxd, _patternCounts))
				//						{
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
				//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
				//						} else
				//						{
				//							double xxxx_weight = contextValues->get(xxxx);
				//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
				//
				//							xxxd_p0 = xxxx_weight * xxxx_prob;
				//							xxxd_backoff = 1.0;
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
				//
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//						}
				//
				//
				//						axxd_backoff = 1.0;
				//						axxd_p0 = xxxd_weight * xxxd_prob;
				//
				//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
				//
				//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
				//					} // axxd
				//
				//					abxd_backoff = 1.0;
				//					abxd_p0 = xbxd_weight * xbxd_prob + axxd_weight * axxd_prob;
				//
				//					if(debug) std::cout << abxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//					if(debug) std::cout << "--" << xbxd_weight << "*" << xbxd_prob << "+" << axxd_weight << "*" << axxd_prob << std::endl;
				//
				//					abxd_prob = probLimitedNaiveHelper(xxxd, Pattern(abxd, 0, 3), abxd, abxd_p0, abxd_backoff, contextCounts, temp_cciPtr);
				//				} // abxd
				//
				//
				//
				//				double axcd_weight = contextValues->get(axcd);
				//				double axcd_backoff = 1.0;
				//				double axcd_prob = 0.0;
				//				double axcd_p0 = 1.0;
				//				if(limitedNaiveOccurs(axcd, _patternCounts))
				//				{
				//					axcd_backoff = 0.0;
				//					axcd_prob = probLimitedNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, axcd_p0, axcd_backoff, contextCounts, temp_cciPtr);
				//
				//					if(debug) std::cout << axcd.tostring(cci->classDecoder) << " found" << std::endl;
				//					if(debug) std::cout << "==" << axcd_prob << std::endl;
				//				} else
				//				{
				//					double axxd_weight = contextValues->get(axxd);
				//					double axxd_backoff = 1.0;
				//					double axxd_prob = 0.0;
				//					double axxd_p0 = 1.0;
				//					if(limitedNaiveOccurs(axxd, _patternCounts))
				//					{
				//						axxd_backoff = 0.0;
				//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
				//
				//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " found" << std::endl;
				//						if(debug) std::cout << "==" << axxd_prob << std::endl;
				//					} else
				//					{
				//						double xxxd_weight = contextValues->get(xxxd);
				//						double xxxd_backoff = 1.0;
				//						double xxxd_prob = 0.0;
				//						double xxxd_p0 = 1.0;
				//						if(limitedNaiveOccurs(xxxd, _patternCounts))
				//						{
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
				//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
				//
				//						} else
				//						{
				//							double xxxx_weight = contextValues->get(xxxx);
				//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
				//
				//							xxxd_p0 = xxxx_weight * xxxx_prob;
				//							xxxd_backoff = 1.0;
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
				//
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//						}
				//
				//
				//						axxd_backoff = 1.0;
				//						axxd_p0 = xxxd_weight * xxxd_prob;
				//
				//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
				//
				//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
				//					} // axxd
				//
				//					double xxcd_weight = contextValues->get(xxcd);
				//					double xxcd_backoff = 1.0;
				//					double xxcd_prob = 0.0;
				//					double xxcd_p0 = 1.0;
				//					if(limitedNaiveOccurs(xxcd, _patternCounts))
				//					{
				//						xxcd_backoff = 0.0;
				//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 1, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
				//
				//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " not found" << std::endl;
				//						if(debug) std::cout << "--" << xxcd_weight << "*" << xxcd_prob << std::endl;
				//					} else
				//					{
				//						double xxxd_weight = contextValues->get(xxxd);
				//						double xxxd_backoff = 1.0;
				//						double xxxd_prob = 0.0;
				//						double xxxd_p0 = 1.0;
				//						if(limitedNaiveOccurs(xxxd, _patternCounts))
				//						{
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//							if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
				//						} else
				//						{
				//							double xxxx_weight = contextValues->get(xxxx);
				//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
				//
				//							xxxd_p0 = xxxx_weight * xxxx_prob;
				//							xxxd_backoff = 1.0;
				//
				//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
				//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
				//
				//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
				//						}
				//
				//
				//						xxcd_backoff = 1.0;
				//						xxcd_p0 = xxxd_weight * xxxd_prob;
				//
				//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " not found" << std::endl;
				//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
				//
				//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 1, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
				//
				//					} // xxcd
				//
				//					axcd_backoff = 1.0;
				//					axcd_p0 = axxd_weight * axxd_prob + xxcd_weight * xxcd_prob;
				//
				//					if(debug) std::cout << axcd.tostring(cci->classDecoder) << " not found" << std::endl;
				//					if(debug) std::cout << "--" << axxd_weight << "*" << axxd_prob << "+" << xxcd_weight << "*" << xxcd_prob << std::endl;
				//
				//					axcd_prob = probLimitedNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, axcd_p0, axcd_backoff, contextCounts, temp_cciPtr);
				//				} // axcd
				//
				//
				//
				//				double abcd_backoff = 1.0;
				//				double abcd_p0 = xbcd_weight * xbcd_prob + axcd_weight * axcd_prob + abxd_weight * abxd_prob;
				//
				//				if(debug) std::cout << abcd.tostring(cci->classDecoder) << " not found" << std::endl;
				//				if(debug) std::cout << "--" << xbcd_weight << "*" << xbcd_prob << "+" << axcd_weight << "*" << axcd_prob << "+" << abxd_weight << "*" << abxd_prob << std::endl;
				//
				//				return probLimitedNaiveHelper(xxxd, Pattern(abcd, 0, 3), abcd, abcd_p0, abcd_backoff, contextCounts, temp_cciPtr);
				//
				//
							}
			}

		};

		double probLimitedNaive(const Pattern& w, const Pattern& context,PatternCounts* _patternCounts,
						ContextCounts* contextCounts, ContextValues* contextValues, LimitedCounts * limitedCounts,
						CoCoInitialiser * const cci = nullptr, const std::string& indent = "") const
		{
			bool debug = true;

			PLNCache plnCache(w, context, _patternCounts);

			if(pattern.size() != 4)
			{
				std::cerr << "Do something: Pattern length is not 4" << std::endl;
			}

			CoCoInitialiser* temp_cciPtr = nullptr;




			double abcd_weight = 1.0;





			if(plnCache.abcd.occurs(_patternCounts))
			{
//				if(debug) std::cout << abcd.tostring(cci->classDecoder) << " found" << std::endl;
//
//				auto it = p.find(Pattern(abcd, 0, 3).reverse());
//				if(it != p.end())
//				{
//					std::cout << "CONTEXT FOR FOUR FOUND" << std::endl;
//
//					// const Pattern& context, const Dish& dish, LimitedInformation* li, const F& p0 = 0.0, const double S = 1.0
//					return it->second.probNaive(Pattern(abcd, 0, 3), xxxd,  1.0, delta, 0.0);
//
//				}
			} else
			{
//
//				double xbcd_weight = contextValues->get(xbcd);
//				double xbcd_backoff = 1.0;
//				double xbcd_prob = 0.0;
//				double xbcd_p0 = 1.0;
				if(plnCache.abcd.occurs(_patternCounts))
				{
//					xbcd_backoff = 0.0;
//					xbcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbcd, 1, 2), xbcd, xbcd_p0, xbcd_backoff, contextCounts, temp_cciPtr);
//
//					if(debug) std::cout << xbcd.tostring(cci->classDecoder) << " found" << std::endl;
//					if(debug) std::cout << "==" << xbcd_prob << std::endl;
				} else
				{
//					double xxcd_weight = contextValues->get(xxcd);
//					double xxcd_backoff = 1.0;
//					double xxcd_prob = 0.0;
//					double xxcd_p0 = 1.0;
					if(plnCache.xxcd.occurs(_patternCounts))
					{
//
//						xxcd_backoff = 0.0;
//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
//
//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " found" << std::endl;
//						if(debug) std::cout << "==" << xxcd_prob << std::endl;
					} else
					{
//						double xxxd_weight = contextValues->get(xxxd);
//						double xxxd_backoff = 1.0;
//						double xxxd_prob = 0.0;
//						double xxxd_p0 = 1.0;
						if(plnCache.xxxd.occurs(_patternCounts))
						{
							plnCache.xxxd.prob();
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
						} else
						{
//							double xxxx_weight = contextValues->get(xxxx);
//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
//
//							xxxd_p0 = xxxx_weight * xxxx_prob;
//							xxxd_backoff = 1.0;
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
//
//
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
						} // xxxx
//
//
//						xxcd_backoff = 1.0;
//						xxcd_p0 = xxxd_weight * xxxd_prob;
//
//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " not found" << std::endl;
//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
//
//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 1, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
//
					} // xxcd
//
//					double xbxd_weight = contextValues->get(xbxd);
//					double xbxd_backoff = 1.0;
//					double xbxd_prob = 0.0;
//					double xbxd_p0 = 1.0;
//					if(limitedNaiveOccurs(xbxd, _patternCounts))
//					{
//						xbxd_backoff = 0.0;
//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 0,2), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
//
//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " found" << std::endl;
//						if(debug) std::cout << "==" << xbxd_prob << std::endl;
//					} else
//					{
//						double xxxd_weight = contextValues->get(xxxd);
//						double xxxd_backoff = 1.0;
//						double xxxd_prob = 0.0;
//						double xxxd_p0 = 1.0;
//						if(limitedNaiveOccurs(xxxd, _patternCounts))
//						{
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
//						} else
//						{
//							double xxxx_weight = contextValues->get(xxxx);
//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
//
//							xxxd_p0 = xxxx_weight * xxxx_prob;
//							xxxd_backoff = 1.0;
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
//
//
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//						}
//
//
//						xxcd_backoff = 1.0;
//						xxcd_p0 = xxxd_weight * xxxd_prob;
//
//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " not found" << std::endl;
//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
//
//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 1, 1), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
//					} // xbxd
//
//					xbcd_backoff = 1.0;
//					xbcd_p0 = xxcd_weight * xxcd_prob + xbxd_weight * xbxd_prob;
//
//					if(debug) std::cout << xbcd.tostring(cci->classDecoder) << " not found" << std::endl;
//					if(debug) std::cout << "--" << xxcd_weight << "*" << xxcd_prob << "+" << xbxd_weight << "*" << xbxd_prob << std::endl;
//
//					xbcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, xbcd_p0, xbcd_backoff, contextCounts, temp_cciPtr);
				} // xbcd
//
//
//
//				double abxd_weight = contextValues->get(abxd);
//				double abxd_backoff = 1.0;
//				double abxd_prob = 0.0;
//				double abxd_p0 = 1.0;
//				if(limitedNaiveOccurs(abxd, _patternCounts))
//				{
//					abxd_backoff = 0.0;
//					abxd_prob = probLimitedNaiveHelper(xxxd, Pattern(abxd, 0, 3), abxd, abxd_p0, abxd_backoff, contextCounts, temp_cciPtr);
//
//					if(debug) std::cout << abxd.tostring(cci->classDecoder) << " found" << std::endl;
//					if(debug) std::cout << "==" << abxd_prob << std::endl;
//				} else
//				{
//					if(debug) std::cout << xbcd.tostring(cci->classDecoder) << " not found" << std::endl;
//					double xbxd_weight = contextValues->get(xbxd);
//					double xbxd_backoff = 1.0;
//					double xbxd_prob = 0.0;
//					double xbxd_p0 = 1.0;
//					if(limitedNaiveOccurs(xbxd, _patternCounts))
//					{
//						xbxd_backoff = 0.0;
//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 0,2), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
//
//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " found" << std::endl;
//						if(debug) std::cout << "==" << xbxd_prob << std::endl;
//					} else
//					{
//						double xxxd_weight = contextValues->get(xxxd);
//						double xxxd_backoff = 1.0;
//						double xxxd_prob = 0.0;
//						double xxxd_p0 = 1.0;
//						if(limitedNaiveOccurs(xxxd, _patternCounts))
//						{
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
//						} else
//						{
//							double xxxx_weight = contextValues->get(xxxx);
//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
//
//							xxxd_p0 = xxxx_weight * xxxx_prob;
//							xxxd_backoff = 1.0;
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
//
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//						}
//
//
//						xbxd_backoff = 1.0;
//						xbxd_p0 = xxxd_weight * xxxd_prob;
//
//						if(debug) std::cout << xbxd.tostring(cci->classDecoder) << " not found" << std::endl;
//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
//
//
//						xbxd_prob = probLimitedNaiveHelper(xxxd, Pattern(xbxd, 0, 2), xbxd, xbxd_p0, xbxd_backoff, contextCounts, temp_cciPtr);
//					} // xbxd
//
//					double axxd_weight = contextValues->get(axxd);
//					double axxd_backoff = 1.0;
//					double axxd_prob = 0.0;
//					double axxd_p0 = 1.0;
//					if(limitedNaiveOccurs(axxd, _patternCounts))
//					{
//						axxd_backoff = 0.0;
//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
//
//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " found" << std::endl;
//						if(debug) std::cout << "==" << axxd_prob << std::endl;
//					} else
//					{
//						double xxxd_weight = contextValues->get(xxxd);
//						double xxxd_backoff = 1.0;
//						double xxxd_prob = 0.0;
//						double xxxd_p0 = 1.0;
//						if(limitedNaiveOccurs(xxxd, _patternCounts))
//						{
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
//						} else
//						{
//							double xxxx_weight = contextValues->get(xxxx);
//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
//
//							xxxd_p0 = xxxx_weight * xxxx_prob;
//							xxxd_backoff = 1.0;
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
//
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//						}
//
//
//						axxd_backoff = 1.0;
//						axxd_p0 = xxxd_weight * xxxd_prob;
//
//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " not found" << std::endl;
//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
//
//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
//					} // axxd
//
//					abxd_backoff = 1.0;
//					abxd_p0 = xbxd_weight * xbxd_prob + axxd_weight * axxd_prob;
//
//					if(debug) std::cout << abxd.tostring(cci->classDecoder) << " not found" << std::endl;
//					if(debug) std::cout << "--" << xbxd_weight << "*" << xbxd_prob << "+" << axxd_weight << "*" << axxd_prob << std::endl;
//
//					abxd_prob = probLimitedNaiveHelper(xxxd, Pattern(abxd, 0, 3), abxd, abxd_p0, abxd_backoff, contextCounts, temp_cciPtr);
//				} // abxd
//
//
//
//				double axcd_weight = contextValues->get(axcd);
//				double axcd_backoff = 1.0;
//				double axcd_prob = 0.0;
//				double axcd_p0 = 1.0;
//				if(limitedNaiveOccurs(axcd, _patternCounts))
//				{
//					axcd_backoff = 0.0;
//					axcd_prob = probLimitedNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, axcd_p0, axcd_backoff, contextCounts, temp_cciPtr);
//
//					if(debug) std::cout << axcd.tostring(cci->classDecoder) << " found" << std::endl;
//					if(debug) std::cout << "==" << axcd_prob << std::endl;
//				} else
//				{
//					double axxd_weight = contextValues->get(axxd);
//					double axxd_backoff = 1.0;
//					double axxd_prob = 0.0;
//					double axxd_p0 = 1.0;
//					if(limitedNaiveOccurs(axxd, _patternCounts))
//					{
//						axxd_backoff = 0.0;
//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
//
//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " found" << std::endl;
//						if(debug) std::cout << "==" << axxd_prob << std::endl;
//					} else
//					{
//						double xxxd_weight = contextValues->get(xxxd);
//						double xxxd_backoff = 1.0;
//						double xxxd_prob = 0.0;
//						double xxxd_p0 = 1.0;
//						if(limitedNaiveOccurs(xxxd, _patternCounts))
//						{
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " found" << std::endl;
//							if(debug) std::cout << "==" << xxxd_prob << std::endl;
//
//						} else
//						{
//							double xxxx_weight = contextValues->get(xxxx);
//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
//
//							xxxd_p0 = xxxx_weight * xxxx_prob;
//							xxxd_backoff = 1.0;
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
//
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//						}
//
//
//						axxd_backoff = 1.0;
//						axxd_p0 = xxxd_weight * xxxd_prob;
//
//						if(debug) std::cout << axxd.tostring(cci->classDecoder) << " not found" << std::endl;
//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
//
//						axxd_prob = probLimitedNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, axxd_p0, axxd_backoff, contextCounts, temp_cciPtr);
//					} // axxd
//
//					double xxcd_weight = contextValues->get(xxcd);
//					double xxcd_backoff = 1.0;
//					double xxcd_prob = 0.0;
//					double xxcd_p0 = 1.0;
//					if(limitedNaiveOccurs(xxcd, _patternCounts))
//					{
//						xxcd_backoff = 0.0;
//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 1, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
//
//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " not found" << std::endl;
//						if(debug) std::cout << "--" << xxcd_weight << "*" << xxcd_prob << std::endl;
//					} else
//					{
//						double xxxd_weight = contextValues->get(xxxd);
//						double xxxd_backoff = 1.0;
//						double xxxd_prob = 0.0;
//						double xxxd_p0 = 1.0;
//						if(limitedNaiveOccurs(xxxd, _patternCounts))
//						{
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
//							if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
//						} else
//						{
//							double xxxx_weight = contextValues->get(xxxx);
//							double xxxx_prob = probLimitedNaiveHelper(Pattern(), Pattern(), Pattern(), 1.0, 1.0, contextCounts, temp_cciPtr);
//
//							xxxd_p0 = xxxx_weight * xxxx_prob;
//							xxxd_backoff = 1.0;
//
//							if(debug) std::cout << xxxd.tostring(cci->classDecoder) << " not found" << std::endl;
//							if(debug) std::cout << "--" << xxxx_weight << "*" << xxxx_prob << std::endl;
//
//							xxxd_prob = probLimitedNaiveHelper(xxxd, Pattern(), xxxd, xxxd_p0, xxxd_backoff, contextCounts, temp_cciPtr);
//						}
//
//
//						xxcd_backoff = 1.0;
//						xxcd_p0 = xxxd_weight * xxxd_prob;
//
//						if(debug) std::cout << xxcd.tostring(cci->classDecoder) << " not found" << std::endl;
//						if(debug) std::cout << "--" << xxxd_weight << "*" << xxxd_prob << std::endl;
//
//						xxcd_prob = probLimitedNaiveHelper(xxxd, Pattern(xxcd, 1, 1), xxcd, xxcd_p0, xxcd_backoff, contextCounts, temp_cciPtr);
//
//					} // xxcd
//
//					axcd_backoff = 1.0;
//					axcd_p0 = axxd_weight * axxd_prob + xxcd_weight * xxcd_prob;
//
//					if(debug) std::cout << axcd.tostring(cci->classDecoder) << " not found" << std::endl;
//					if(debug) std::cout << "--" << axxd_weight << "*" << axxd_prob << "+" << xxcd_weight << "*" << xxcd_prob << std::endl;
//
//					axcd_prob = probLimitedNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, axcd_p0, axcd_backoff, contextCounts, temp_cciPtr);
//				} // axcd
//
//
//
//				double abcd_backoff = 1.0;
//				double abcd_p0 = xbcd_weight * xbcd_prob + axcd_weight * axcd_prob + abxd_weight * abxd_prob;
//
//				if(debug) std::cout << abcd.tostring(cci->classDecoder) << " not found" << std::endl;
//				if(debug) std::cout << "--" << xbcd_weight << "*" << xbcd_prob << "+" << axcd_weight << "*" << axcd_prob << "+" << abxd_weight << "*" << abxd_prob << std::endl;
//
//				return probLimitedNaiveHelper(xxxd, Pattern(abcd, 0, 3), abcd, abcd_p0, abcd_backoff, contextCounts, temp_cciPtr);
//
//
			}





//
//			// -----------------------------
//			// 0
//			double xxxx_prob = probFullNaiveHelper(Pattern(), Pattern(), xxxx, 0, temp_cciPtr);
//			if(debug) std::cout << "xxxx p: " << xxxx_prob << std::endl;
//
//			// 1
//			double xxxd_prob = probFullNaiveHelper(xxxd, Pattern(), xxxd, xxxx_prob, temp_cciPtr);
//			if(debug) std::cout << "xxxd p: " << xxxd_prob << std::endl;
//
//			// 2
//			double axxd_prob = probFullNaiveHelper(xxxd, Pattern(axxd, 0, 3), axxd, xxxd_prob, temp_cciPtr);
//
//			if(debug) std::cout << "axxd p: " << axxd_prob << " with weight: " << axxd_weight << std::endl;
//			double xbxd_prob = probFullNaiveHelper(xxxd, Pattern(xbxd, 0, 2), xbxd, xxxd_prob, temp_cciPtr);
//
//			if(debug) std::cout << "xbxd p: " << xbxd_prob << " with weight: " << xbxd_weight << std::endl;
//			double xxcd_prob = probFullNaiveHelper(xxxd, Pattern(xxcd, 0, 1), xxcd, xxxd_prob, temp_cciPtr);
//
//			if(debug) std::cout << "xxcd p: " << xxcd_prob << " with weight: " << xxcd_weight << std::endl;
//
//			double c2_prob = (axxd_prob * axxd_weight + xbxd_prob * xbxd_weight + xxcd_prob * xxcd_weight) / (axxd_weight + xbxd_weight + xxcd_weight);
//
//			// 3
//			double abxd_prob = probFullNaiveHelper(xxxd, Pattern(abxd, 0, 3), abxd, c2_prob, temp_cciPtr);
//
//			if(debug) std::cout << "abxd p: " << abxd_prob << " with weight: " << abxd_weight << std::endl;
//			double axcd_prob = probFullNaiveHelper(xxxd, Pattern(axcd, 0, 3), axcd, c2_prob, temp_cciPtr);
//
//			if(debug) std::cout << "axcd p: " << axcd_prob << " with weight: " << axcd_weight << std::endl;
//			double xbcd_prob = probFullNaiveHelper(xxxd, Pattern(xbcd, 0, 2), xbcd, c2_prob, temp_cciPtr);
//
//			if(debug) std::cout << "xbcd p: " << xbcd_prob << " with weight: " << xbcd_weight << std::endl;
//
//			double c3_prob = (abxd_prob * abxd_weight + axcd_prob * axcd_weight + xbcd_prob * xbcd_weight) / (abxd_weight + axcd_weight + xbcd_weight);
//
//			// 4
//			double abcd_prob = probFullNaiveHelper(xxxd, Pattern(abcd, 0, 3), abcd, c3_prob, temp_cciPtr);
//
//			if(debug) std::cout << "abcd p: " << abcd_prob << " with weight: " << abcd_weight << std::endl;
//
//			return abcd_prob;

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
