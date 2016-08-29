#ifndef STRATEGIES_H_
#define STRATEGIES_H_

#include "utils.h"
#include "hpyplm.h"

class BackoffStrategy
{
public:
	bool debug = false;

    int files = 0;
    unsigned long lines = 0;
    double llh = 0.0;
    unsigned long long count = 0;
    unsigned long long oovs = 0;

    unsigned long fLines = 0;
    double fLLH = 0.0;
    unsigned long long fCount = 0;
    unsigned long long fOOVs = 0;

    SNCBWCoCoInitialiser& cci;

    std::string baseOutputName;
    std::string outputClassFileName;
    std::string outputCorpusFileName;
    std::string outputPatternModelFileName;
    std::string outputProbabilitiesFileName;
    std::string outputFile;

    my_ostream* mout;
    std::ofstream probsFile;

    cpyp::PYPLM<kORDER>& lm;

    BackoffStrategy(SNCBWCoCoInitialiser& _cci, cpyp::PYPLM<kORDER>& _lm);

    virtual std::string strategyName();

    virtual double prob(const Pattern& focus, const Pattern& context, const std::string& focusString);


    int nextFile();

    int nextLine();

    void done();

    void printFileResults();

    void printResults();
};

class BackoffStrategies
{
public:
    std::vector<BackoffStrategy*> backoffStrategies;

    void prob(const Pattern& focus, const Pattern& context, const std::string& focusString);

    void addBackoffStrategy(BackoffStrategy* strategy);

    void nextFile();

    void nextLine();

    void printFileResults();

    void printResults();

    void done();

    ~BackoffStrategies();
};

class NgramBackoffStrategy : public BackoffStrategy
{
public:
    std::string strategyName();
    SNCBWProgramOptions& po;

    NgramBackoffStrategy(SNCBWProgramOptions& _po, SNCBWCoCoInitialiser& _cci, cpyp::PYPLM<kORDER>& _lm);

    ~NgramBackoffStrategy();

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString);
};

class FullBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName();
    SNCBWProgramOptions& po;

    FullBackoffStrategy(SNCBWProgramOptions& _po,
    					SNCBWCoCoInitialiser& _cci,
						cpyp::PYPLM<kORDER>& _lm,
						ContextCounts* _contextCounts,
						ContextValues* _contextValues);

    ~FullBackoffStrategy();

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString);
};

class FullNaiveBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName();
    SNCBWProgramOptions& po;

    FullNaiveBackoffStrategy(SNCBWProgramOptions& _po,
    					SNCBWCoCoInitialiser& _cci,
						cpyp::PYPLM<kORDER>& _lm,
						ContextCounts* _contextCounts,
						ContextValues* _contextValues);

    ~FullNaiveBackoffStrategy();

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString);
};

class LimitedBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName();
    SNCBWProgramOptions& po;
    PatternCounts* patternCounts;

    LimitedBackoffStrategy(SNCBWProgramOptions& _po,
    		SNCBWCoCoInitialiser& _cci,
    		cpyp::PYPLM<kORDER>& _lm,
			PatternCounts* _patternCounts,
			ContextCounts* _contextCounts,
			ContextValues* _contextValues);

    virtual ~LimitedBackoffStrategy();

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString);
};

class LimitedNaiveBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName();
    SNCBWProgramOptions& po;
    PatternCounts* patternCounts;

    LimitedNaiveBackoffStrategy(SNCBWProgramOptions& _po,
    		SNCBWCoCoInitialiser& _cci,
    		cpyp::PYPLM<kORDER>& _lm,
			PatternCounts* _patternCounts,
			ContextCounts* _contextCounts,
			ContextValues* _contextValues);

    virtual ~LimitedNaiveBackoffStrategy();

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString);
};

#endif
