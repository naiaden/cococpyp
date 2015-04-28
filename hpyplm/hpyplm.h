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

    if(p.size() >= 1) {
        for(int i = 1; i < p.size(); ++i) {
            Pattern q = p.addskip(std::pair<int, int>(i,1));
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
	void increment(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder = nullptr) {

                Pattern rev = context.reverse();
                Pattern pattern = Pattern(rev, 0, N-1);
                Pattern shortened_context = pattern.reverse();

                std::string indentation = std::string(kORDER-N, '\t');
                if(decoder != nullptr) {
                    std::cout << N << indentation << "I: " << w.tostring(*decoder) << " | " << shortened_context.tostring(*decoder) << std::endl;
                }

		const double bo = backoff.prob(w, context, decoder, false);

		auto it = p.find(pattern);
		if (it == p.end()) {
			it = p.insert(make_pair(pattern, crp<Pattern>(0.8, 0))).first;
			tr.insert(&it->second); // add to resampler
		}

		if (it->second.increment(w, bo, eng, false/*cs == "o.a. uit" && fs == ":"*/)) {
			backoff.increment(w, context, eng, decoder);
		}

  /*              
                std::vector<Pattern> skipped_patterns = generateSkips(shortened_context);
                for(Pattern skipped_context : skipped_patterns) {
                    const double s_bo = backoff.prob(w, skipped_context, decoder);
                    auto s_rev = skipped_context.reverse();

                    auto s_it = p.find(s_rev);
                    if(s_it == p.end()) {
                        s_it = p.insert(make_pair(s_rev, crp<Pattern>(0.8, 0))).first;
                        tr.insert(&s_it->second);
                    }
                    if(s_it->second.increment(w, bo, eng, false)) {
                        backoff.increment(w, skipped_context, eng, decoder);
                    }
                }
    */
	}

	template<typename Engine>
	void decrement(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
                Pattern rev = context.reverse();
                Pattern pattern = Pattern(rev, 0, N-1);
                Pattern shortened_context = pattern.reverse();

		auto it = p.find(pattern);
		assert(it != p.end());

		if (it->second.decrement(w, eng)) {
			backoff.decrement(w, context, eng, decoder);
		}
/*
                std::vector<Pattern> skipped_patterns = generateSkips(shortened_context);
                for(Pattern skipped_context : skipped_patterns) {
                    Pattern s_rev = skipped_context.reverse();

                    auto s_it = p.find(s_rev);
                    assert(s_it != p.end());

                    if(s_it->second.decrement(w, eng)) {
                        backoff.decrement(w, skipped_context, eng, decoder);
                    }
                }
*/
	}

	double prob(const Pattern& w, const Pattern& context, ClassDecoder * const decoder = nullptr, bool backoff_to_skips = false, std::map<Pattern, int> * patternAdded = nullptr, std::map<Pattern, std::set<Pattern> > * patternSpawned = nullptr) const {
                Pattern pattern = Pattern(context.reverse(), 0, N-1);
                Pattern shortened_context = pattern.reverse();

/*                // "dynamic" backoff
                if(patternAdded != nullptr && patternSpawned != nullptr) {
                    std::vector<Pattern> all_subngrams;
                    context.subngrams(all_subngrams);
                    std::set<Pattern> subngrams(all_subngrams.begin(), all_subngrams.end());
                    
                    double prob = 0;
                    for(sng : subngrams) {
                        prob += (patternAdded[sng] * 1.0) / patternSpawned[sng].size() * ;
                    }
                }
*/

                std::string indentation = std::string(kORDER-N, '\t');
                if(decoder != nullptr) {
                    std::cout << N << indentation << "P: " << w.tostring(*decoder) << " | " << shortened_context.tostring(*decoder) << std::endl;
                }

		const double bo = backoff.prob(w, context, decoder, backoff_to_skips);

		auto it = p.find(pattern);
		if (it == p.end()) {

                    double average;
                    
                    if(backoff_to_skips) {

                        // okay, ngram is not available
                        // time for some backoff
                        std::vector<Pattern> skipped_patterns = generateSkips(shortened_context);
                        for(Pattern skipped_context : skipped_patterns) {
                            Pattern s_rev = skipped_context.reverse();

                            average += ((prob(w, skipped_context,decoder,backoff_to_skips))/(skipped_patterns.size()+1));
                        }
                        average += bo/(skipped_patterns.size()+1);

                        if(decoder != nullptr) {
                            std::cout << indentation << "BO: " << average << " (original bo: " << bo << ")" << std::endl;
                        }



                        return average;
 
                    } else {
                        return bo;
                    }
                }


                double ret = it->second.prob(w, bo);
                 
                if(decoder != nullptr) {
                    std::cout << N << indentation << "CRP: " << ret << std::endl;
                }

                return ret;
		//return it->second.prob(w, bo);
	}

        double avg( std::initializer_list<double> list )
        {
            double s = 0.0;
            double count = 0.0;
            for( auto elem : list )
            {
                s += elem ;
                ++count;
            }
            if(count) return s/count;
            return 0.0;
        }

        // |p| = 4

        // w5 | w1, w2, w3, w4
        double j15(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j15(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder) << std::endl; }

            double r7 = j7(w, c, decoder);
            double r11 = j11(w,c, decoder);
            double r13 = j13(w,c, decoder);
            double r14 = j14(w,c, decoder);
            return avg( {prob(w,c),r7, r11, r13, r14} );
        }
        // w5 | w1, w2, w3, __
        double j14(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j14(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = c.addskip(std::pair<int, int>(3,1));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r6 = j6(w, c, decoder);
            double r10 = j10(w,c,decoder);
            double r12 = j12(w,c,decoder);
            return avg( {prob(w,p), r6, r10, r12} );
        }
        // w5 | w1, w2, __, w4
        double j13(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j13(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = c.addskip(std::pair<int, int>(2,1));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r5 = j5(w, c, decoder);
            double r9 = j9(w,c,decoder);
            double r12 = j12(w,c,decoder);
            return avg( {prob(w,p), r5, r9, r12} );
        }
        // w5 | w1, w2, __, __
        double j12(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j12(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = c.addskip(std::pair<int, int>(2,2));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r4 = j4(w,c,decoder);
            double r8 = j8(w,c,decoder);
            return avg( {prob(w,p), r4, r8} );
        }
        // w5 | w1, __, w3, w4
        double j11(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j11(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = c.addskip(std::pair<int, int>(1,1));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r3 = j3(w,c,decoder);
            double r9 = j9(w,c,decoder);
            double r10 = j10(w,c,decoder);
            return avg( {prob(w,p), r3, r9, r10} );
        }
        // w5 | w1, __, w3, __
        double j10(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j10(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            std::vector<std::pair<int, int> > skips;
            skips.push_back(std::pair<int, int>(1,1));
            skips.push_back(std::pair<int, int>(3,1));
            Pattern p = c.addskips(skips);

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r2 = j2(w,c,decoder);
            double r8 = j8(w,c,decoder);
            return avg( {prob(w,p), r2, r8} );
        }
        // w5 | w1, __, __, w4
        double j9(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j9(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = c.addskip(std::pair<int, int>(1,2));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r1 = j1(w,c,decoder);
            double r8 = j8(w,c,decoder);
            return avg( {prob(w,p), r1, r8} );
        }
        // w5 | w1, __, __, __
        double j8(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 4) return backoff.j8(w, c, decoder);

            if(decoder != nullptr) { std::cerr << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = c.addskip(std::pair<int, int>(1,3));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r0 = j0(w,c,decoder);
            return avg( {prob(w, p) ,r0} );
        }


        // w5 |     w2, w3, w4
        double j7(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 3) return backoff.j7(w, c, decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j7: " <<  w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern(c, kORDER-4,3);

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r3 = j3(w,c,decoder);
            double r5 = j5(w,c,decoder);
            double r6 = j6(w,c,decoder);
            return avg( {prob(w,p), r3, r5, r6} );
        }
        // w5 |     w2, w3, __
        double j6(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 3) return backoff.j6(w, c, decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j6: " << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern(c, kORDER-4,3).addskip(std::pair<int, int>(2,1));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r2 = j2(w,c,decoder);
            double r4 = j4(w,c,decoder);
            return avg( {prob(w,p), r2, r4} );
        }
        // w5 |     w2, __, w4
        double j5(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 3) return backoff.j5(w, c, decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j5: " << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern(c, kORDER-4, 3).addskip(std::pair<int, int>(1,1));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r1 = j1(w,c,decoder);
            double r4 = j4(w,c,decoder);
            return avg( {prob(w,p), r1, r4} );
        }
        // w5 |     w2, __, __
        double j4(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 3) return backoff.j4(w, c, decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j4: " << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern(c, kORDER-4, 3).addskip(std::pair<int, int>(1,2));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r0 = j0(w,c,decoder);
            return avg( { prob(w, c) , r0 } );
        }


        // w5 |         w3, w4
        double j3(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 2) return backoff.j3(w, c, decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j3: " << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern(c, kORDER-4+1, 2);

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r1 = j1(w,c,decoder);
            double r2 = j2(w,c,decoder);
            return avg( {prob(w,p), r1, r2} );
        }
        // w5 |         w3, __
        double j2(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 2) return backoff.j2(w, c, decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j2: " << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern(c, kORDER-4+1, 2).addskip(std::pair<int, int>(1,1));

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r0 = j0(w,c,decoder);
            return avg( { prob(w, p), r0 } );
        }
        // w5 |             w4
        double j1(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 2) return backoff.j1(w, c, decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j1: " << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern(c, kORDER-4+2, 1);

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            double r0 = j0(w,c,decoder);
            return avg( { prob(w, p) , r0 } );
        }


        // w5 |
        double j0(const Pattern& w, const Pattern& c, ClassDecoder * const decoder = nullptr) {
            if(N > 1) return backoff.j0(w,c,decoder);

            if(decoder != nullptr) { std::cerr << "[" << N << "]j0: " << w.tostring(*decoder) << " | " << c.tostring(*decoder); }

            Pattern p = Pattern();

            if(decoder != nullptr) { std::cerr << " --> " << p.tostring(*decoder) << std::endl; }

            return prob(w, p);
        }

        double glm_prob(const Pattern& w, const Pattern& context, ClassDecoder * const decoder = nullptr) const {
                Pattern pattern = Pattern(context.reverse(), 0, N-1);
                Pattern new_context = pattern.reverse();

                std::string indentation = std::string(5-N, '\t');
                std::string prob_string = "p(" + w.tostring(*decoder) + "|" + new_context.tostring(*decoder) + ")";

                std::cout << indentation << "Entering glm on level " << N << ": " << prob_string << std::endl;
                const double level_bo = backoff.glm_prob(w, context, decoder);

                std::cout << indentation << "\tLevel back off: " << level_bo << std::endl;

                if(N >= 3) {
                    double skipgram_bo = 0.0;
                    for(int j = 1; j < N; ++j) {
                        Pattern skipped_context = pattern.reverse().addskip(std::pair<int,int>(j,1));
                        double sb = glm_prob(w, skipped_context, decoder);
                        std::cout << indentation << "\t[" << j << "]SB: " << sb << " for: " << skipped_context.tostring(*decoder) << std::endl;
                    }
                }



                std::cout << indentation << "Leaving glm" << std::endl;
                auto it = p.find(pattern);
                if(it == p.end()) {
                    return level_bo;
                }
                return it->second.prob(w, level_bo);

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
