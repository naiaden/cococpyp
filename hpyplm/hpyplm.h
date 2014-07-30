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

                Pattern rev = context.reverse();
                Pattern pattern = Pattern(rev, 0, N-1);
                //Pattern pattern = Pattern(context.reverse(), 0, N-1);

		auto it = p.find(pattern);
		if (it == p.end()) {
                //        if(N==3 && p.size() < 5) std::cerr << "[" << p.size();
			it = p.insert(make_pair(pattern, crp<Pattern>(0.8, 0))).first;
                //        if(N==3 && p.size() < 6) std::cerr << " -> " << p.size() << "]: " << pattern.hash() << std::endl;
			tr.insert(&it->second); // add to resampler
		}

		if (it->second.increment(w, bo, eng, false/*cs == "o.a. uit" && fs == ":"*/)) {
			backoff.increment(w, context, eng, decoder);
		}
	}

	template<typename Engine>
	void decrement(const Pattern& w, const Pattern& context, Engine& eng, ClassDecoder * const decoder = nullptr) {
                Pattern rev = context.reverse();
                Pattern pattern = Pattern(rev, 0, N-1);
                //Pattern pattern = Pattern(context.reverse(), 0, N-1);

                //if(N==3) std::cerr << "c: " << context.hash() << "\nw: " << w.hash() << std::endl;
                //if(N==3) std::cerr << "------ " << pattern.hash() << std::endl;


		auto it = p.find(pattern);
		assert(it != p.end());

		if (it->second.decrement(w, eng)) {
			backoff.decrement(w, context, eng, decoder);
		}
	}

	double prob(const Pattern& w, const Pattern& context, ClassDecoder * const decoder = nullptr) const {
                //if(N == 3 && decoder != nullptr) {
                //    std::cerr << ">>\t[" << w.tostring(*decoder);
                //    std::cerr << ", " << context.tostring(*decoder);
                //    std::cerr << "]" << std::endl;
                //}
		const double bo = backoff.prob(w, context, decoder);

                //if(/*N == 3 &&*/ decoder != nullptr) {
                //    std::cerr << "\t" << N << "\t" << bo << std::endl;
                //}

                Pattern pattern = Pattern(context.reverse(), 0, N-1);

		auto it = p.find(pattern);
		if (it == p.end()) {
			return bo;
                        }
		return it->second.prob(w, bo);
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
        double j15(const Pattern& w, const Pattern& p, const Pattern& c) {
            if(N > 4) return backoff.j15(w, p, c);

            double r7 = j7(w, Pattern(p, 1, 3), c);
            double r11 = j11(w,p,c);
            double r13 = j13(w,p,c);
            double r14 = j14(w,p,c);
            return prob(w, c) + 0.8*avg( {r7, r11, r13, r14} );
        }
        // w5 | w1, w2, w3, __
        double j14(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 4) return backoff.j14(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(3,1));
            double r6 = j6(w, Pattern(p, 1, 3), c);
            double r10 = j10(w,p,c);
            double r12 = j12(w,p,c);
            return prob(w, c) + 0.8*avg( {r6, r10, r12} );
        }
        // w5 | w1, w2, __, w4
        double j13(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 4) return backoff.j13(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(2,1));
            double r5 = j5(w, Pattern(p, 1, 3), c);
            double r9 = j9(w,p,c);
            double r12 = j12(w,p,c);
            return prob(w, c) + 0.8*avg( {r5, r9, r12} );
        }
        // w5 | w1, w2, __, __
        double j12(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 4) return backoff.j12(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(3,2));
            double r4 = j4(Pattern(p, 1, 3));
            double r8 = j8(w,p,c);
            return prob(w, c) + 0.8*avg( {r4, r8} );
        }
        // w5 | w1, __, w3, w4
        double j11(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 4) return backoff.j11(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(1,1));
            double r3 = j3(Pattern(p, 2, 2));
            double r9 = j9(w,p,c);
            double r10 = j10(w,p,c);
            return prob(w, c) + 0.8*avg( {r3, r9, r10} );
        }
        // w5 | w1, __, w3, __
        double j10(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 4) return backoff.j10(w, pp);

            std::vector<std::pair<int, int> > skips;
            skips.push_back(std::pair<int, int>(1,1));
            skips.push_back(std::pair<int, int>(3,1));
            Pattern p = pp.addskips(skips);
            double r2 = j2(Pattern(p, 2, 2));
            double r8 = j8(w,p,c);
            return prob(w, c) + 0.8*avg( {r2, r8} );
        }
        // w5 | w1, __, __, w4
        double j9(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 4) return backoff.j9(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(1,2));
            double r1 = j1(Pattern(p, 3, 1));
            double r8 = j8(w,p,c);
            return prob(w, c) + 0.8*avg( {r1, r8} );
        }
        // w5 | w1, __, __, __
        double j8(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 4) return backoff.j8(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(1,3));
            double r0 = j0(Pattern());
            return prob(w, c) + 0.8*r0;
        }

        // w5 |     w2, w3, w4
        double j7(const Pattern& w, const Pattern& p, const Pattern& c) {
            if(N > 3) return backoff.j7(w, p);

            double r3 = j3(Pattern(p, 1, 2));
            double r5 = j5(w,p,c);
            double r6 = j6(w,p,c);
            return prob(w, c) + 0.8*avg( {r3, r5, r6} );
        }
        // w5 |     w2, w3, __
        double j6(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 3) return backoff.j6(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(2,1));
            double r2 = j2(Pattern(p, 1, 2));
            double r4 = j4(w,p,c);
            return prob(w, c) + 0.8*avg( {r2, r4} );
        }
        // w5 |     w2, __, w4
        double j5(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 3) return backoff.j5(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(1,1));
            double r1 = j1(Pattern(p, 2, 1));
            double r4 = j4(w,p,c);
            return prob(w, c) + 0.8*avg( {r1, r4} );
        }
        // w5 |     w2, __, __
        double j4(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 3) return backoff.j4(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(1,2));
            double r0 = j0(Pattern());
            return prob(w, c) + 0.8*r0;
        }

        // w5 |         w3, w4
        double j3(const Pattern& w, const Pattern& p, const Pattern& c) {
            if(N > 2) return backoff.j3(w, p);

            double r1 = j1(Pattern(p, 1, 1));
            double r2 = j2(w,p,c);
            return prob(w, c) + 0.8*avg( {r1, r2} );
        }
        // w5 |         w3, __
        double j2(const Pattern& w, const Pattern& pp, const Pattern& c) {
            if(N > 2) return backoff.j2(w, pp);

            Pattern p = pp.addskip(std::pair<int, int>(1,1));
            double r0 = j0(Pattern());
            return prob(w, c) + 0.8*r0;
        }
        // w5 |             w4
        double j1(const Pattern& w, const Pattern& p, const Pattern& c) {
            if(N > 2) return backoff.j1(w, p);
            double r0 = j0(Pattern());
            return prob(w, c) + 0.8*r0;
        }

        // w5 |
        double j0(const Pattern& w, const Pattern& p, const Pattern& c) {
            if(N > 1) return backoff.j0(w,p,c);
            return backoff.prob(w, p);
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
