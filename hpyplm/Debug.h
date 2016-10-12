/*
 * Debug.h
 *
 *  Created on: Oct 12, 2016
 *      Author: louis
 */

#ifndef HPYPLM_DEBUG_H_
#define HPYPLM_DEBUG_H_

#include <string>
#include <ostream>
#include <iostream>

enum class DebugLevel { NONE = 0, PATTERN = 2, SUBPATTERN = 4, ALL = 100 };


class Debug {
public:
	static Debug& getInstance()
	{
		static Debug instance;
		return instance;
	}

	Debug(Debug const&) = delete;
	void operator=(Debug const&) = delete;

	DebugLevel get();
	void set(DebugLevel level);
	void set(const std::string& level);

	std::string toString(DebugLevel debugLevel) const;
	std::string toString() const;

	bool doDebug(DebugLevel currentLevel) const
	{
		return currentLevel >= debugLevel && debugLevel != DebugLevel::NONE;
	}

//	template<typename T> Debug& operator<<(DebugLevel dl)
//		{
//			coutLevel = dl;
//			return *this;
//		}

	void setOutputLevel(DebugLevel dl)
	{
		coutLevel = dl;
		std::cout << "Setting output level to " << toString(dl) << std::endl;
	}

	template<typename T> Debug& operator<<(const T& something)
	{
		if(doDebug(coutLevel))
		{
			std::cout << something;
		}
		return *this;
	}

	typedef std::ostream& (*stream_function)(std::ostream&);
	Debug& operator<<(stream_function func)
	{
		func(std::cout);
		return *this;
	}

	Debug& operator<<(DebugLevel dl)
	{
		coutLevel = dl;
		return *this;
	}
private:
	DebugLevel debugLevel = DebugLevel::NONE;
	DebugLevel coutLevel = DebugLevel::NONE;

	Debug() {}

};


#endif /* HPYPLM_DEBUG_H_ */
