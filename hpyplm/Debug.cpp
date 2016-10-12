/*
 * Debug.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: louis
 */


#include "Debug.h"

std::string Debug::toString(DebugLevel debugLevel) const
{
	switch (debugLevel) {
		case DebugLevel::NONE:
			return "NONE";
		case DebugLevel::PATTERN:
			return "PATTERN";
		case DebugLevel::SUBPATTERN:
			return "SUBPATTERN";
		case DebugLevel::ALL:
			return "ALL";
		default:
			return "unknown";
	}
}

std::string Debug::toString() const
{
	return toString(debugLevel);
}

DebugLevel Debug::get()
{
	return debugLevel;
}

void Debug::set(const std::string& level)
{
	if(level == "PATTERN") debugLevel = DebugLevel::PATTERN;
	if(level == "SUBPATTERN") debugLevel = DebugLevel::SUBPATTERN;
	if(level == "ALL") debugLevel = DebugLevel::ALL;

	std::cout << "Debug output level is " << toString(debugLevel) << std::endl;
}

void Debug::set(DebugLevel level)
{
	if(doDebug(DebugLevel::ALL))
	{
		std::cout << "Changing debug level from " << toString(debugLevel) << " to " << toString(level) << std::endl;
	}
	debugLevel = level;
}
