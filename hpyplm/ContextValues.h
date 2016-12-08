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

class PatternCounts;

class ContextValues
{
protected:
	std::vector<double> providedValues;
	bool provideValues = false;
public:
	virtual double get(const Pattern& pattern, CoCoInitialiser * const cci = nullptr) = 0;
	virtual std::string name() const = 0;


	std::vector<double> getProvidedValues() { return providedValues; }
};





class UniformCounts : public ContextValues
{
public:
	std::string name() const;

	UniformCounts(SNCBWCoCoInitialiser& cci, bool trackValues = false);

	double get(const Pattern& pattern, CoCoInitialiser * const cci);
};

class MLECounts : public ContextValues
{
public:
	std::string name() const;

	std::unordered_map<Pattern, double> mleCounts;

	MLECounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts, bool trackValues = false);

	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);

	double get(const Pattern& pattern, CoCoInitialiser * const cci);
};

class EntropyCounts : public ContextValues
{
	public:
	std::string name() const;

	std::unordered_map<Pattern, double> entropyCounts;

	double emptyEntropy = 1.0;
	long int V = 0;

	EntropyCounts(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts, bool trackValues = false);


	double get(const Pattern& context,
			CoCoInitialiser * const cci = nullptr);



	void initialise(SNCBWCoCoInitialiser& cci, PatternCounts* patternCounts);

};






#endif /* HPYPLM_CONTEXTVALUES_H_ */
