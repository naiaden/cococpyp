#ifndef STRATEGIES_H_
#define STRATEGIES_H_

#include "utils.h"

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

    BackoffStrategy(SNCBWCoCoInitialiser& _cci, cpyp::PYPLM<kORDER>& _lm)
        : cci(_cci), lm(_lm)
    {
        
    }

    virtual std::string strategyName()
    {
        return "I HAVE NO NAME";
    }

    virtual double prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
    {
        return 0.0;
    }


    int nextFile()
    {
        ++files;

        lines += fLines;
        llh += fLLH;
        count += fCount;
        oovs += fOOVs;

        fLines = 0;
        fLLH = 0.0;
        fCount = 0;
        fOOVs = 0;

        return files;
    }

    int nextLine()
    {
        return ++fLines;
    }

    void done()
    {
        lines += fLines;
        llh += fLLH;
        count += fCount;
        oovs += fOOVs;
    }

    void printFileResults()
    {
        double lprob = (-fLLH * log(2)) / log(10); // in cpyp: (-llh * log(2) / log(10))
        *mout << "        Lines: " << fLines << std::endl;
        *mout << "  Log_10 prob: " << lprob << std::endl;
        *mout << "        Count: " << fCount-fOOVs << std::endl;
        *mout << "         OOVs: " << fOOVs << std::endl;
        *mout << "Cross-Entropy: " << (fLLH/fCount) << std::endl;
        *mout << "   Perplexity: " << pow(2, fLLH/fCount) << std::endl;
    }

    void printResults()
    {
        *mout << "\n===== OVERALL STATISTICS for " << strategyName() << std::endl;

        double lprob = (-llh * log(2)) / log(10); // in cpyp: (-llh * log(2) / log(10))
        *mout << "        Files: " << files << std::endl;
        *mout << "        Lines: " << lines << std::endl;
        *mout << "  Log_10 prob: " << lprob << std::endl;
        *mout << "        Count: " << count-oovs << std::endl;
        *mout << "         OOVs: " << oovs << std::endl;
        *mout << "Cross-Entropy: " << (llh/count) << std::endl;
        *mout << "   Perplexity: " << pow(2, llh/count) << std::endl;
    }
};

class BackoffStrategies
{
public:
    std::vector<BackoffStrategy*> backoffStrategies;

    void prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
    {
        for(BackoffStrategy* bs: backoffStrategies)
        {
            bs->prob(focus, context, focusString);
        }
    }

    void addBackoffStrategy(BackoffStrategy* strategy)
    {
        backoffStrategies.push_back(strategy);
    }

    void nextFile()
    {
        for(BackoffStrategy* bs: backoffStrategies)
        {
            bs->nextFile();
        }
    }

    void nextLine()
    {
        for(BackoffStrategy* bs: backoffStrategies)
        {
            bs->nextLine();
        }
    }

    void printFileResults()
    {
        for(BackoffStrategy* bs: backoffStrategies)
        {
            bs->printFileResults();
        }
    }

    void printResults()
    {
        for(BackoffStrategy* bs: backoffStrategies)
        {
            bs->printResults();
        }
    }

    void done()
    {
        for(BackoffStrategy* bs: backoffStrategies)
        {
            bs->done();
        }
    }

    ~BackoffStrategies()
    {
        for(std::vector<BackoffStrategy*>::iterator it = backoffStrategies.begin();
            it != backoffStrategies.end(); ++it)
        {
            delete(*it);
        }
        backoffStrategies.clear();
    }
};

class NgramBackoffStrategy : public BackoffStrategy
{
public:
    std::string strategyName()
    {
        return "ngram";
    }
    SNCBWProgramOptions& po;

    NgramBackoffStrategy(SNCBWProgramOptions& _po, SNCBWCoCoInitialiser& _cci, cpyp::PYPLM<kORDER>& _lm) : BackoffStrategy(_cci, _lm), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;
       
        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n);
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);

        debug = false;
    }

    ~NgramBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
    {
        //std::cout << strategyName() << "[" << focus.tostring(classDecoder)
        //                            << "/" << context.tostring(classDecoder) 
        //                            << "] " << focusString
        //                            << std::endl;

    	if(debug) std::cout << " Entering ngram backoff\n";

        double lp = 0.0;
        std::string fS = focusString;

        if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
        {
        	if(debug) std::cout << "+++ Processing [" << context.tostring(cci.classDecoder) << " " << focus.tostring(cci.classDecoder) << std::endl;
            lp = log2(lm.prob(focus, context, &cci));
            fS = focus.tostring(cci.classDecoder);
            if(debug) std::cout << "--- logprob = " << lp << std::endl;
        } else // oov
        {
            ++fOOVs;
            probsFile << "***";
        }

        if(debug) std::cout << " writing to probs file...";

        probsFile << "p(" << fS << " |"
                  << context.tostring(cci.classDecoder) << ") = "
                  << std::fixed << std::setprecision(20) << lp 
                  << std::endl;



        fLLH -= lp;
        ++fCount;

        if(debug) std::cout << " done\n";

        return lp;
    }
};

class FullBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName()
    {
        return "full";
    }
    SNCBWProgramOptions& po;

    FullBackoffStrategy(SNCBWProgramOptions& _po,
    					SNCBWCoCoInitialiser& _cci,
						cpyp::PYPLM<kORDER>& _lm,
						ContextCounts* _contextCounts,
						ContextValues* _contextValues)
    : BackoffStrategy(_cci, _lm), contextCounts(_contextCounts), contextValues(_contextValues), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;
       
        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n) + "_" + _contextValues->name();
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);

        debug = true;
    }

    ~FullBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
    {
    	if(debug) std::cout << " Entering full backoff\n";

        double lp = 0.0;
        std::string fS = focusString;

        if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
        {
        	if(debug) std::cout << "+++ Processing [" << context.tostring(cci.classDecoder) << " " << focus.tostring(cci.classDecoder) << std::endl;
            lp = log2(lm.probFull(focus, context, contextCounts, contextValues, &cci));
            fS = focus.tostring(cci.classDecoder);
            if(debug) std::cout << "--- logprob = " << lp << std::endl;
        } else // oov
        {
            ++fOOVs;
            probsFile << "***";
        }

        if(debug) std::cout << " writing to probs file...";

//        std::cout << "\np(" << fS << " |"
        probsFile << "p(" << fS << " |"
                  << context.tostring(cci.classDecoder) << ") = "
                  << std::fixed << std::setprecision(20) << lp 
                  << std::endl;

        fLLH -= lp;
        ++fCount;

        double lwhatever = (-fLLH * log(2)) / log(10);
//        std::cout << "-LLH:" << -fLLH << "\tW/E:" << lwhatever << std::endl;
        if(!std::isnormal(lwhatever))
        {
        	std::cout << "lp is not normal" << std::endl;
        	exit( 8);
        }

        if(debug) std::cout << " done\n";

        return lp;
    }
};

class FullNaiveBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName()
    {
        return "fullnaive";
    }
    SNCBWProgramOptions& po;

    FullNaiveBackoffStrategy(SNCBWProgramOptions& _po,
    					SNCBWCoCoInitialiser& _cci,
						cpyp::PYPLM<kORDER>& _lm,
						ContextCounts* _contextCounts,
						ContextValues* _contextValues)
    : BackoffStrategy(_cci, _lm), contextCounts(_contextCounts), contextValues(_contextValues), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;

        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n) + "_" + _contextValues->name();
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);

        debug = false;
    }

    ~FullNaiveBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
    {
    	if(debug) std::cout << " Entering " << strategyName() << " backoff\n";

        double lp = 0.0;
        std::string fS = focusString;

        if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
        {
        	if(debug) std::cout << "+++ Processing [" << context.tostring(cci.classDecoder) << " " << focus.tostring(cci.classDecoder) << std::endl;
            lp = log2(lm.probFullNaive(focus, context, contextCounts, contextValues, &cci));
            fS = focus.tostring(cci.classDecoder);
            if(debug) std::cout << "--- logprob = " << lp << std::endl;
        } else // oov
        {
            ++fOOVs;
            probsFile << "***";
        }

        if(debug) std::cout << " writing to probs file...";

//        std::cout << "\np(" << fS << " |"
        probsFile << "p(" << fS << " |"
                  << context.tostring(cci.classDecoder) << ") = "
                  << std::fixed << std::setprecision(20) << lp
                  << std::endl;

        fLLH -= lp;
        ++fCount;

        double lwhatever = (-fLLH * log(2)) / log(10);
//        std::cout << "-LLH:" << -fLLH << "\tW/E:" << lwhatever << std::endl;
        if(!std::isnormal(lwhatever))
        {
        	std::cout << "lp is not normal" << std::endl;
        	exit( 8);
        }

        if(debug) std::cout << " done\n";

        return lp;
    }
};

class LimitedBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName()
    {
        return "limited";
    }
    SNCBWProgramOptions& po;
    PatternCounts* patternCounts;

    LimitedBackoffStrategy(SNCBWProgramOptions& _po,
    		SNCBWCoCoInitialiser& _cci,
    		cpyp::PYPLM<kORDER>& _lm,
			PatternCounts* _patternCounts,
			ContextCounts* _contextCounts,
			ContextValues* _contextValues)
    : BackoffStrategy(_cci, _lm), patternCounts(_patternCounts), contextCounts(_contextCounts), contextValues(_contextValues), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;
       
        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n) + "_" + _contextValues->name();
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);
    }

    virtual ~LimitedBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
        {
            double lp = 0.0;
            std::string fS = focusString;

            if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
            {
                lp = log2(lm.probLimited(focus, context, patternCounts, contextCounts, contextValues, &cci));
                fS = focus.tostring(cci.classDecoder);
            } else // oov
            {
                ++fOOVs;
                probsFile << "***";
            }

    //        std::cout << "\np(" << fS << " |"
            probsFile << "p(" << fS << " |"
                      << context.tostring(cci.classDecoder) << ") = "
                      << std::fixed << std::setprecision(20) << lp
                      << std::endl;

            fLLH -= lp;
            ++fCount;

            double lwhatever = (-fLLH * log(2)) / log(10);
    //        std::cout << "-LLH:" << -fLLH << "\tW/E:" << lwhatever << std::endl;
            if(!std::isnormal(lwhatever))
            {
            	exit( 8);
            }

            return lp;
        }
};

class LimitedNaiveBackoffStrategy : public BackoffStrategy
{
	ContextCounts* contextCounts;
	ContextValues* contextValues;

public:
    std::string strategyName()
    {
        return "limitednaive";
    }
    SNCBWProgramOptions& po;
    PatternCounts* patternCounts;

    LimitedNaiveBackoffStrategy(SNCBWProgramOptions& _po,
    		SNCBWCoCoInitialiser& _cci,
    		cpyp::PYPLM<kORDER>& _lm,
			PatternCounts* _patternCounts,
			ContextCounts* _contextCounts,
			ContextValues* _contextValues)
    : BackoffStrategy(_cci, _lm), patternCounts(_patternCounts), contextCounts(_contextCounts), contextValues(_contextValues), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;

        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n) + "_" + _contextValues->name();
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);
    }

    virtual ~LimitedNaiveBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
        {
            double lp = 0.0;
            std::string fS = focusString;

            if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
            {
                lp = log2(lm.probLimitedNaive(focus, context, patternCounts, contextCounts, contextValues, &cci));
                fS = focus.tostring(cci.classDecoder);
            } else // oov
            {
                ++fOOVs;
                probsFile << "***";
            }

            std::cout << "\np(" << fS << " |"
//            probsFile << "p(" << fS << " |"
                      << context.tostring(cci.classDecoder) << ") = "
                      << std::fixed << std::setprecision(20) << lp
                      << std::endl;

            fLLH -= lp;
            ++fCount;

            double lwhatever = (-fLLH * log(2)) / log(10);
    //        std::cout << "-LLH:" << -fLLH << "\tW/E:" << lwhatever << std::endl;
            if(!std::isnormal(lwhatever))
            {
            	exit( 8);
            }

            return lp;
        }
};

#endif
