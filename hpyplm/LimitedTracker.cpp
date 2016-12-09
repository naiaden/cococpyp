/*
 * LimitedTracker.cpp
 *
 *  Created on: Dec 7, 2016
 *      Author: onrust
 */

#include "LimitedTracker.h"
#include <iostream>
#include <numeric>


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

double LimitedTracker::registerWeight(PatternPattern pp, double weight)
{
	weights[pp].push_back(weight);
}

void LimitedTracker::reset()
{
	hits.clear();
	weights.clear();
}

void LimitedTracker::print(/*const std::string& file*/)
{
	std::cout << "HITS:" << std::endl;
	for (const auto& kv : hits) {
		std::cout << "\t" << ppToString(kv.first) << " has " << kv.second << " hits" << std::endl;
	}

	std::cout << "WEIGHTS:" << std::endl;
	for (const auto& kv : weights) {
		std::cout << "\t" << ppToString(kv.first) << ": " << accumulate( kv.second.begin(), kv.second.end(), 0.0)/kv.second.size() << std::endl;
	}
}
