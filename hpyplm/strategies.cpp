#include "strategies.h"

#include "utils.h"
#include "hpyplm.h"


BackoffStrategy::BackoffStrategy(SNCBWCoCoInitialiser& _cci, cpyp::PYPLM<kORDER>& _lm)
	: cci(_cci), lm(_lm)
{

}

std::string BackoffStrategy::strategyName()
{
	return "I HAVE NO NAME";
}

double BackoffStrategy::prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
{
	return 0.0;
}


int BackoffStrategy::nextFile()
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

	lLLH = 0.0;

	return files;
}

int BackoffStrategy::nextLine()
{
	sentenceFile << fLines << "\t " << lLLH << std::endl;


	lLLH = 0.0;

	return ++fLines;
}

void BackoffStrategy::done()
{
	lines += fLines;
	llh += fLLH;
	count += fCount;
	oovs += fOOVs;
}

void BackoffStrategy::printFileResults()
{
	double lprob = (-fLLH * log(2)) / log(10); // in cpyp: (-llh * log(2) / log(10))
	*mout << "         File: " << files << std::endl;
	*mout << "        Lines: " << fLines << std::endl;
	*mout << "  Log_10 prob: " << lprob << std::endl;
	*mout << "        Count: " << fCount-fOOVs << std::endl;
	*mout << "         OOVs: " << fOOVs << std::endl;
	*mout << "Cross-Entropy: " << (fLLH/fCount) << std::endl;
	*mout << "   Perplexity: " << pow(2, fLLH/fCount) << std::endl;
	*mout << "               " << std::endl;
}

void BackoffStrategy::printResults()
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











double BackoffStrategies::prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->prob(focus, context, focusString);
	}
}

void BackoffStrategies::addBackoffStrategy(BackoffStrategy* strategy)
{
	backoffStrategies.push_back(strategy);
}

void BackoffStrategies::nextFile()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->nextFile();
	}
}

void BackoffStrategies::nextLine()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->nextLine();
	}
}

void BackoffStrategies::printFileResults()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->printFileResults();
	}
}

void BackoffStrategies::printResults()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->printResults();
	}
}

void BackoffStrategies::done()
{
	for(BackoffStrategy* bs: backoffStrategies)
	{
		bs->done();
	}
}

BackoffStrategies::~BackoffStrategies()
{
	for(std::vector<BackoffStrategy*>::iterator it = backoffStrategies.begin();
		it != backoffStrategies.end(); ++it)
	{
		delete(*it);
	}
	backoffStrategies.clear();
}









std::string NgramBackoffStrategy::strategyName()
{
	return "ngram";
}

NgramBackoffStrategy::NgramBackoffStrategy(SNCBWProgramOptions& _po, SNCBWCoCoInitialiser& _cci, cpyp::PYPLM<kORDER>& _lm) : BackoffStrategy(_cci, _lm), po(_po)
{
	std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;

	baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n);
	outputProbabilitiesFileName = baseOutputName + ".probs";
	outputSentenceProbabilitiesFileName = baseOutputName + ".sentences";
	outputFile = baseOutputName + ".output";

	std::cout << "Writing backoff output to " << outputFile << std::endl;

	mout = new my_ostream(outputFile);
	probsFile.open(outputProbabilitiesFileName);
	sentenceFile.open(outputSentenceProbabilitiesFileName);



	debug = false;
}

NgramBackoffStrategy::~NgramBackoffStrategy()
{
	probsFile.close();
	sentenceFile.close();
	delete mout;
}

double NgramBackoffStrategy::prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
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

	lLLH -= lp;

	if(debug) std::cout << " done\n";

	return lp;
}















std::string FullNaiveBackoffStrategy::strategyName()
{
	return "fullnaive";
}

FullNaiveBackoffStrategy::FullNaiveBackoffStrategy(SNCBWProgramOptions& _po,
					SNCBWCoCoInitialiser& _cci,
					cpyp::PYPLM<kORDER>& _lm,
					ContextCounts* _contextCounts,
					ContextValues* _contextValues)
: BackoffStrategy(_cci, _lm), contextCounts(_contextCounts), contextValues(_contextValues), po(_po)
{
	std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;

	baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n) + "_" + _contextValues->name();
	outputProbabilitiesFileName = baseOutputName + ".probs";
	outputSentenceProbabilitiesFileName = baseOutputName + ".sentences";
	outputFile = baseOutputName + ".output";

	std::cout << "Writing backoff output to " << outputFile << std::endl;

	mout = new my_ostream(outputFile);
	probsFile.open(outputProbabilitiesFileName);
	sentenceFile.open(outputSentenceProbabilitiesFileName);

	debug = false;
}

FullNaiveBackoffStrategy::~FullNaiveBackoffStrategy()
{
	probsFile.close();
	sentenceFile.close();
	delete mout;
}

double FullNaiveBackoffStrategy::prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
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
	lLLH -= lp;
	++fCount;

	if(debug) std::cout << " done\n";

	return lp;
}











std::string BasicFullNaiveBackoffStrategy::strategyName()
{
	return "basicfullnaive" + contextValues->name();
}

BasicFullNaiveBackoffStrategy::BasicFullNaiveBackoffStrategy(SNCBWProgramOptions& _po,
					SNCBWCoCoInitialiser& _cci,
					cpyp::PYPLM<kORDER>& _lm,
					ContextCounts* _contextCounts,
					ContextValues* _contextValues)
: BackoffStrategy(_cci, _lm), contextCounts(_contextCounts), contextValues(_contextValues), po(_po)
{
	std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;

	debug = false;
}

BasicFullNaiveBackoffStrategy::~BasicFullNaiveBackoffStrategy()
{
}

// not the log prob
double BasicFullNaiveBackoffStrategy::prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
{
	if(debug) std::cout << " Entering " << strategyName() << " backoff\n";

	double p = 0.0;
	std::string fS = focusString;

	if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
	{
		if(debug) std::cout << "+++ Processing (" << (context+focus).size() << ") [" << context.tostring(cci.classDecoder) << " " << focus.tostring(cci.classDecoder) << "]" << std::endl;

		if(context.size() == 0) p = lm.probFullNaive1(focus, context, contextCounts, contextValues, &cci);
		else if(context.size() == 1) p = lm.probFullNaive2(focus, context, contextCounts, contextValues, &cci);
		else if(context.size() == 2) p = lm.probFullNaive3(focus, context, contextCounts, contextValues, &cci);
		else p = lm.probFullNaive(focus, context, contextCounts, contextValues, &cci);

		fS = focus.tostring(cci.classDecoder);
		if(debug) std::cout << "--- prob = " << p << std::endl;
	} else // oov
	{
	}

	if(debug) std::cout << " done\n";

	return p;
}














std::string LimitedNaiveBackoffStrategy::strategyName()
{
	return "limitednaive";
}

LimitedNaiveBackoffStrategy::LimitedNaiveBackoffStrategy(SNCBWProgramOptions& _po,
		SNCBWCoCoInitialiser& _cci,
		cpyp::PYPLM<kORDER>& _lm,
		PatternCounts* _patternCounts,
		ContextCounts* _contextCounts,
		ContextValues* _contextValues,
		LimitedCounts* _limitedCounts)
: BackoffStrategy(_cci, _lm), patternCounts(_patternCounts), contextCounts(_contextCounts), contextValues(_contextValues), limitedCounts(_limitedCounts), po(_po)
{
	std::cout << "Initialising backoff strategy: " << strategyName() << std::endl;

	baseOutputName = _po.generalBaseOutputName + "_" + strategyName() + "_" + std::to_string(_po.n) + "_" + _contextValues->name();
	outputProbabilitiesFileName = baseOutputName + ".probs";
	outputSentenceProbabilitiesFileName = baseOutputName + ".sentences";
	outputFile = baseOutputName + ".output";

	std::cout << "Writing backoff output to " << outputFile << std::endl;

	mout = new my_ostream(outputFile);
	probsFile.open(outputProbabilitiesFileName);
	sentenceFile.open(outputSentenceProbabilitiesFileName);


}

LimitedNaiveBackoffStrategy::~LimitedNaiveBackoffStrategy()
{
	probsFile.close();
	sentenceFile.close();
	delete mout;
}

double LimitedNaiveBackoffStrategy::prob(const Pattern& focus, const Pattern& context, const std::string& focusString)
{
	double lp = 0.0;
	std::string fS = focusString;

	if(focusString.empty()) // That means we can derive its string from the class decoder, and it's not oov
	{
		lp = log2(lm.probLimitedNaive(focus, context, lm, patternCounts, contextCounts, contextValues, limitedCounts, &cci));
		fS = focus.tostring(cci.classDecoder);
	} else // oov
	{
		++fOOVs;
		probsFile << "***";
	}

            probsFile << "p(" << fS << " |"
			  << context.tostring(cci.classDecoder) << ") = "
			  << std::fixed << std::setprecision(20) << lp
			  << std::endl;

	fLLH -= lp;
	lLLH -= lp;
	++fCount;

	return lp;
}

