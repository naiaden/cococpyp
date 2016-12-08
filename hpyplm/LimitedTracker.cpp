/*
 * LimitedTracker.cpp
 *
 *  Created on: Dec 7, 2016
 *      Author: onrust
 */

#include "LimitedTracker.h"
#include <iostream>

long int LimitedTracker::registerHit(PatternPattern pp)
{
	hits[pp]++;
}

std::string ppToString(PatternPattern pp)
{
	switch (pp) {
		case PatternPattern::abcd:
			return "abcd";
		case PatternPattern::abxd:
					return "abxd";
		case PatternPattern::axcd:
					return "axcd";
		case PatternPattern::xbcd:
					return "xbcd";
		case PatternPattern::axxd:
					return "axxd";
		case PatternPattern::xbxd:
					return "xbxd";
		case PatternPattern::xxcd:
					return "xxcd";
		case PatternPattern::xxxd:
					return "xxxd";
		default:
			return "xxxx";
	}


}

void LimitedTracker::reset()
{
	hits.clear();
}

void LimitedTracker::print()
{
 for (const auto& kv : hits) {
	 std::cout << ppToString(kv.first) << " has " << kv.second << " hits" << std::endl;
	}
}
