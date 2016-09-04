/*
 * PLNCache.h
 *
 *  Created on: Aug 31, 2016
 *      Author: louis
 */

#ifndef HPYPLM_PLNCACHE_H_
#define HPYPLM_PLNCACHE_H_

//#include "CoCoInitialiser.h"
class CoCoInitialiser;
//#include "PatternCounts.h"
class PatternCounts;
class ContextCounts;
class ContextValues;
class LimitedCounts;
#include <pattern.h>

//#include "hpyplm.h"
//#include "PatternCache.h"
class P_ABCD;
class P_ABXD;
class P_AXCD;
class P_XBCD;
class P_AXXD;
class P_XBXD;
class P_XXCD;
class P_XXXD;
class P_XXXX;

class PLNCache {
public:

	P_ABCD* abcd;
	P_ABXD* abxd;
	P_AXCD* axcd;
	P_XBCD* xbcd;
	P_AXXD* axxd;
	P_XBXD* xbxd;
	P_XXCD* xxcd;
	P_XXXD* xxxd;
	P_XXXX* xxxx;

	PatternCounts* pc;
	ContextCounts* cc;
	ContextValues* cv;
	LimitedCounts * lc;

	CoCoInitialiser * cci;

	bool debug = false;

	PLNCache(const Pattern& w, const Pattern& context, PatternCounts* pc, ContextCounts* cc, ContextValues* cv, LimitedCounts * lc, CoCoInitialiser * const cci = nullptr);

	~PLNCache();

};

#endif /* HPYPLM_PLNCACHE_H_ */
