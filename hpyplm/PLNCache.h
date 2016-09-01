/*
 * PLNCache.h
 *
 *  Created on: Aug 31, 2016
 *      Author: louis
 */

#ifndef HPYPLM_PLNCACHE_H_
#define HPYPLM_PLNCACHE_H_

#include "PatternCache.h"
#include "CoCoInitialiser.h"

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

	PLNCache(const Pattern& w, const Pattern& context, PatternCounts* pc, ContextCounts* cc, ContextValues* cv, LimitedCounts * lc, CoCoInitialiser * const cci = nullptr)
		: pc(pc), cc(cc), cv(cv), lc(lc), cci(cci)
	{
		if(cci) std::cout << "Creating a PLNCache for: " << context.tostring(cci->classDecoder) << " " << w.tostring(cci->classDecoder) << std::endl;

		abcd = new P_ABCD(this, w, context);
		abxd = new P_ABXD(this, w, context);
		axcd = new P_AXCD(this, w, context);
		xbcd = new P_XBCD(this, w, context);

		axxd = new P_AXXD(this, w, context);
		xbxd = new P_XBXD(this, w, context);
		xxcd = new P_XXCD(this, w, context);

		xxxd = new P_XXXD(this, w, context);

		xxxx = new P_XXXX(this, w, context);

		abcd->compute();
	}

	~PLNCache() {
		delete xxxx;
//		delete xxxd;
	}

};

#endif /* HPYPLM_PLNCACHE_H_ */
