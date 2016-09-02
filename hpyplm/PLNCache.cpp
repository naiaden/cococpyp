/*
 * PLNCache.cpp
 *
 *  Created on: Sep 2, 2016
 *      Author: louis
 */

#include "PLNCache.h"

#include "PatternCache.h"
//#include "CoCoInitialiser.h"
//#include "PatternCounts.h"
//
//#include "hpyplm.h"


PLNCache::PLNCache(const Pattern& w, const Pattern& context, PatternCounts* pc, ContextCounts* cc, ContextValues* cv, LimitedCounts * lc, CoCoInitialiser * const cci)
	: pc(pc), cc(cc), cv(cv), lc(lc), cci(cci)
{
	abcd = new P_ABCD(this, w, context);
	abxd = new P_ABXD(this, w, context);
	axcd = new P_AXCD(this, w, context);
	xbcd = new P_XBCD(this, w, context);

	axxd = new P_AXXD(this, w, context);
	xbxd = new P_XBXD(this, w, context);
	xxcd = new P_XXCD(this, w, context);

	xxxd = new P_XXXD(this, w, context);

	xxxx = new P_XXXX(this, w, context);

//		abcd->compute();
}

PLNCache::~PLNCache() {
	delete xxxx;
	delete xxxd;
	delete xxcd;
	delete xbxd;
	delete axxd;
	delete xbcd;
	delete axcd;
	delete abxd;
	delete abcd;
}


