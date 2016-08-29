/*
 * ContextValues.h
 *
 *  Created on: Mar 11, 2016
 *      Author: louis
 */

#ifndef HPYPLM_CONTEXTVALUES_H_
#define HPYPLM_CONTEXTVALUES_H_


#include "CoCoInitialiser.h"
#include "utils.h"

#include "PatternCounts.cpp"


class ContextValues
{
public:
	virtual double get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) const = 0;
	virtual std::string name() const = 0;
};





class UniformCounts : public ContextValues
{
public:
	std::string name() const;

	UniformCounts(SNCBWCoCoInitialiser& cci);

	double get(const Pattern& pattern, CoCoInitialiser * const cci) const;
};

class MLECounts : public ContextValues
{
public:
	std::string name() const;

	std::unordered_map<Pattern, double> mleCounts;

	MLECounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);

	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);

	double get(const Pattern& pattern, CoCoInitialiser * const cci) const;
};

class EntropyCounts : public ContextValues
{
	public:
	std::string name() const;

	std::unordered_map<Pattern, double> entropyCounts;

	double emptyEntropy = 1.0;
	long int V = 0;

	EntropyCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);


	double get(const Pattern& context,
			CoCoInitialiser * const cci = nullptr) const;



	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);

};






#endif /* HPYPLM_CONTEXTVALUES_H_ */
