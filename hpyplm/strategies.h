#ifndef STRATEGIES_H_
#define STRATEGIES_H_

#include "utils.h"

class BackoffStrategy
{
public:
    int files = 0;
    unsigned long lines = 0;
    double llh = 0.0;
    unsigned long long count = 0;
    unsigned long long oovs = 0;

    unsigned long fLines = 0;
    double fLLH = 0.0;
    unsigned long long fCount = 0;
    unsigned long long fOOVs = 0;

    ClassDecoder& classDecoder;

    std::string baseOutputName;
    std::string outputClassFileName;
    std::string outputCorpusFileName;
    std::string outputPatternModelFileName;
    std::string outputProbabilitiesFileName;
    std::string outputFile;

    my_ostream* mout;
    std::ofstream probsFile;

    cpyp::PYPLM<kORDER>& lm;

    BackoffStrategy(ClassDecoder& _classDecoder, cpyp::PYPLM<kORDER>& _lm)
        : classDecoder(_classDecoder), lm(_lm)
    {
        
    }

    virtual std::string strategyName()
    {
        return "I HAVE NO NAME";
    }

    virtual double prob(const Pattern& focus, const Pattern& context, std::string& focusString)
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
        *mout << "\n===== OVERALL STATISTICS" << std::endl;

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

    void prob(const Pattern& focus, const Pattern& context, std::string& focusString)
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

    NgramBackoffStrategy(SNCBWProgramOptions& _po, ClassDecoder& _classDecoder, cpyp::PYPLM<kORDER>& _lm) : BackoffStrategy(_classDecoder, _lm), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;
       
        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n);
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);
    }

    ~NgramBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, std::string& focusString)
    {
        double lp = 0.0;

        if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
        {
            lp = log(lm.prob(focus, context, &classDecoder));
            focusString = focus.tostring(classDecoder);
        } else // oov
        {
            ++fOOVs;
            probsFile << "***";
        }

        probsFile << "p(" << focusString << " |"
                  << context.tostring(classDecoder) << ") = "
                  << std::fixed << std::setprecision(20) << lp 
                  << std::endl;

        fLLH -= lp;
        ++fCount;

        return lp;
    }
};

class FullBackoffStrategy : public BackoffStrategy
{
public:
    std::string strategyName()
    {
        return "full";
    }
    SNCBWProgramOptions& po;

    LimitedBackoffStrategy(SNCBWProgramOptions& _po, ClassDecoder& _classDecoder, cpyp::PYPLM<kORDER>& _lm) : BackoffStrategy(_classDecoder, _lm), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;
       
        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n);
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);
    }

    ~FullBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, std::string& focusString)
    {
        double lp = 0.0;

        if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
        {
            lp = lm.probFull(focus, context, &classDecoder);
            focusString = focus.tostring(classDecoder);
        } else // oov
        {
            ++fOOVs;
            probsFile << "***";
        }

        probsFile << "p(" << focusString << " |"
                  << context.tostring(classDecoder) << ") = "
                  << std::fixed << std::setprecision(20) << lp 
                  << std::endl;

        fLLH -= lp;
        ++fCount;

        return lp;
    }
};

class LimitedBackoffStrategy : public BackoffStrategy
{
public:
    std::string strategyName()
    {
        return "limited";
    }
    SNCBWProgramOptions& po;

    LimitedBackoffStrategy(SNCBWProgramOptions& _po, ClassDecoder& _classDecoder, cpyp::PYPLM<kORDER>& _lm) : BackoffStrategy(_classDecoder, _lm), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;
       
        baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n);
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

        std::cout << "Writing backoff output to " << outputFile << std::endl;

        mout = new my_ostream(outputFile);
        probsFile.open(outputProbabilitiesFileName);
    }

    ~LimitedBackoffStrategy()
    {
        probsFile.close();
        delete mout;
    }

    double prob(const Pattern& focus, const Pattern& context, std::string& focusString)
    {
        double lp = 0.0;

        if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
        {
            lp = lm.probLimited(focus, context, &classDecoder);
            focusString = focus.tostring(classDecoder);
        } else // oov
        {
            ++fOOVs;
            probsFile << "***";
        }

        probsFile << "p(" << focusString << " |"
                  << context.tostring(classDecoder) << ") = "
                  << std::fixed << std::setprecision(20) << lp 
                  << std::endl;

        fLLH -= lp;
        ++fCount;

        return lp;
    }
};

#endif
