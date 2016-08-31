/*
 * PatternCache.h
 *
 *  Created on: Aug 31, 2016
 *      Author: louis
 */

#ifndef HPYPLM_PATTERNCACHE_H_
#define HPYPLM_PATTERNCACHE_H_

class PLNCache;
#include <pattern.h>
class PatternCounts;
class ContextValues;
class ContextCounts;
class LimitedCounts;

	class PatternCache
		{
		protected:
			bool occurs_ = false;

			double backoff_ = 0.0;
			double prob_ = 0.0;
			double weight_ = 0.0;
			double p0_ = 0.0;

			bool computed = false;

			Pattern focus;
			Pattern context;
			Pattern pattern;

			PatternCounts* pc;
			ContextCounts* cc;
			ContextValues* cv;
			LimitedCounts* lc;

			PLNCache* parent;

		public:
			PatternCache(PLNCache* parent, const Pattern& _focus, const Pattern& _context) : parent(parent), focus(_focus), context(_context), pattern(_context + _focus), pc(pc), cc(cc), cv(cv), lc(lc) {} ;

			double compute() {};

			Pattern& getPattern();
			double getWeight();
			double getProb();
		};

		class P_XXXX : public PatternCache
		{
		public:
			P_XXXX(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = Pattern();
			}

			double compute();
		};

		class P_XXXD : public PatternCache
		{
		public:
			P_XXXD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = Pattern();
			}

			double compute();
		};

		class P_XXCD : public PatternCache
		{
		public:
			P_XXCD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = Pattern(c, 2, 2);
			}

			double compute();
		};

		class P_XBXD : public PatternCache
		{
		public:
			P_XBXD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = Pattern(c, 1, 2).addskip(std::pair<int, int>(1, 1));
			}

			double compute();
		};

		class P_AXXD : public PatternCache
		{
		public:
			P_AXXD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = c.addskip(std::pair<int, int>(1, 2));
			}

			double compute();
		};

		class P_XBCD : public PatternCache
		{
		public:
			P_XBCD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = Pattern(c, 1, 2);
			}

			double compute();
		};

		class P_AXCD : public PatternCache
		{
		public:
			P_AXCD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = c.addskip(std::pair<int, int>(1, 1));
			}

			double compute();
		};

		class P_ABXD : public PatternCache
		{
		public:
			P_ABXD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{
				context = c.addskip(std::pair<int, int>(2, 1));
			}

			double compute();
		};

		class P_ABCD : public PatternCache
		{
		public:
			P_ABCD(PLNCache* parent, const Pattern& f, const Pattern& c) : PatternCache(parent, f,c)
			{

			};

			double compute();
		};


#endif /* HPYPLM_PATTERNCACHE_H_ */
