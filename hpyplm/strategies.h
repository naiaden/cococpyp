#ifndef STRATEGIES_H_
#define STRATEGIES_H_

#include "utils.h"

class BackoffStrategy
{
public:
    int files = 0;
    int lines = 0;
    double llh = 0.0;
    unsigned count = 0;
    unsigned oovs = 0;

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
    
    int nextFile()
    {
        return ++files;
    }

    int nextLine()
    {
        return ++lines;
    }

    void printResults()
    {
        double lprob = (-llh * log(2)) / log(10); // in cpyp: (-llh * log(2) / log(10))
        std::cout << "        Files: " << files << std::endl;
        std::cout << "        Lines: " << lines << std::endl;
        std::cout << "  Log_10 prob: " << lprob << std::endl;
        std::cout << "        Count: " << count-oovs << std::endl;
        std::cout << "         OOVs: " << oovs << std::endl;
        std::cout << "Cross-Entropy: " << (llh/count) << std::endl;
        std::cout << "   Perplexity: " << pow(2, llh/count) << std::endl;
    }
};

class NgramBackoffStrategy : public BackoffStrategy
{
public:
    const std::string strategyName = "ngram";
    SNCBWProgramOptions& po;

    NgramBackoffStrategy(SNCBWProgramOptions& _po, ClassDecoder& _classDecoder, cpyp::PYPLM<kORDER>& _lm) : BackoffStrategy(_classDecoder, _lm), po(_po)
    {
        std::cout << "Initialising backoff strategy: " << strategyName << std::endl;
        
        baseOutputName = _po.baseOutputName + "_" + strategyName + "_" + std::to_string(_po.n);
//        outputClassFileName = baseOutputName + ".cls";
//        outputPatternModelFileName = baseOutputName + ".patternmodel";
//        outputCorpusFileName = baseOutputname + ".dat";
        outputProbabilitiesFileName = baseOutputName + ".probs";
        outputFile = baseOutputName + ".output";

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

        std::cout << "!! focus size = " << focus.size() << " context size = " << context.size() << std::endl;

        if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
        {
            lp = lm.prob(focus, context, &classDecoder);
            focusString = focus.tostring(classDecoder);
        } else // oov
        {
            ++oovs;
            probsFile << "***";
        }

        /*probsFile*/ std::cout << "p(" << focusString << " |"
                  << context.tostring(classDecoder) << ") = "
                  << std::fixed << std::setprecision(20) << lp 
                  << std::endl;

        llh -= lp;
        ++count;

        return lp;
    }
};

#endif
