#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <chrono>

#include <iomanip> // put_time
#include <string>  // string

#include <iostream>
#include <fstream>

#include <pattern.h>

#include <string>
#include <sstream>
#include <vector>

#include "utils.h"

void sssplit(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
    ss.str(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> ssplit(const std::string &s, char delim) {
	std::vector<std::string> elems;
    sssplit(s, delim, elems);
    return elems;
}

//enum class Backoff { NGRAM, LIM, LIMENT, LIMMLE, LIMUNI, FULL, FULLENT, FULLMLE, FULLUNI, ENT, MLE, UNI, ALL };
bool backoffIn(Backoff b, const std::vector<Backoff> bs)
{
	for(Backoff backoff: bs)
	{
		if(b == backoff) return true;
		if(b == Backoff::ALL) return true;

		if(b == Backoff::LIM && (backoff == Backoff::LIMENT || backoff == Backoff::LIMMLE || backoff == Backoff::LIMUNI)) return true;
		if(b == Backoff::FULL && (backoff == Backoff::FULLENT || backoff == Backoff::FULLMLE || backoff == Backoff::FULLUNI)) return true;

		if(b == Backoff::ENT && (backoff == Backoff::FULLENT || backoff == Backoff::LIMENT)) return true;
		if(b == Backoff::MLE && (backoff == Backoff::FULLMLE || backoff == Backoff::LIMMLE)) return true;
		if(b == Backoff::UNI && (backoff == Backoff::FULLUNI || backoff == Backoff::LIMUNI)) return true;
	}
	return false;
}

std::vector<Backoff> fromString(const std::string& s) {
	std::vector<Backoff> backoffs;

	std::vector<std::string> sBackoffs = ssplit(s, '.');

	for(const std::string& sb : sBackoffs)
	{
		if(sb.compare("ngram") == 0 || sb.compare("all") == 0) backoffs.push_back(Backoff::NGRAM);

		if(sb.compare("lim") == 0 || sb.compare("lim-ent") == 0 || sb.compare("all") == 0 || sb.compare("ent") == 0) backoffs.push_back(Backoff::LIMENT);
		if(sb.compare("lim") == 0 || sb.compare("lim-mle") == 0 || sb.compare("all") == 0 || sb.compare("mle") == 0) backoffs.push_back(Backoff::LIMMLE);
		if(sb.compare("lim") == 0 || sb.compare("lim-uni") == 0 || sb.compare("all") == 0 || sb.compare("uni") == 0) backoffs.push_back(Backoff::LIMUNI);

		if(sb.compare("full") == 0 || sb.compare("full-ent") == 0 || sb.compare("all") == 0 || sb.compare("ent") == 0) backoffs.push_back(Backoff::FULLENT);
		if(sb.compare("full") == 0 || sb.compare("full-mle") == 0 || sb.compare("all") == 0 || sb.compare("mle") == 0) backoffs.push_back(Backoff::FULLMLE);
		if(sb.compare("full") == 0 || sb.compare("full-uni") == 0 || sb.compare("all") == 0 || sb.compare("uni") == 0) backoffs.push_back(Backoff::FULLUNI);
	}

    return backoffs;
}

// enum class Backoff { NGRAM, LIM, LIMENT, LIMMLE, LIMUNI, FULL, FULLENT, FULLMLE, FULLUNI, ALL };
std::string backoffsToString(const std::vector<Backoff>& bs) {

	std::set<std::string> backoffSet;

	std::stringstream s;
	for(Backoff b: bs)
	{
		if(b == Backoff::NGRAM   || b == Backoff::ALL)	backoffSet.insert("ngram");
		if(b == Backoff::LIMENT  || b == Backoff::ALL)	backoffSet.insert("lim-ent");
		if(b == Backoff::LIMMLE  || b == Backoff::ALL)	backoffSet.insert("lim-mle");
		if(b == Backoff::LIMUNI  || b == Backoff::ALL)	backoffSet.insert("lim-uni");
		if(b == Backoff::FULLENT || b == Backoff::ALL)	backoffSet.insert("full-ent");
		if(b == Backoff::FULLMLE || b == Backoff::ALL)	backoffSet.insert("full-mle");
		if(b == Backoff::FULLUNI || b == Backoff::ALL)	backoffSet.insert("full-uni");

		if(b == Backoff::LIM || b == Backoff::ALL)
		{
			backoffSet.insert("lim-mle"); 	backoffSet.insert("lim-ent");	backoffSet.insert("lim-uni");
		}

		if(b == Backoff::FULL || b == Backoff::ALL)
		{
			backoffSet.insert("full-mle");	backoffSet.insert("full-ent");	backoffSet.insert("full-uni");
		}

		if(b == Backoff::MLE || b == Backoff::ALL)
		{
			backoffSet.insert("full-mle");	backoffSet.insert("lim-mle");
		}

		if(b == Backoff::ENT || b == Backoff::ALL)
		{
			backoffSet.insert("full-ent");	backoffSet.insert("lim-ent");
		}

		if(b == Backoff::UNI || b == Backoff::ALL)
		{
			backoffSet.insert("full-uni");	backoffSet.insert("lim-uni");
		}
	}

	bool append = false;

	for(std::string backoffString : backoffSet)
	{
		if(append)
		{
			s << ".";
		}

		s << backoffString;
		append = true;
	}

    return s.str();
}


std::vector<std::string> wsSplit(std::string const &input) {
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer),
                                 std::istream_iterator<std::string>()};
    return ret;
}








    QueryTimeStatsPrinter::QueryTimeStatsPrinter(my_ostream* m)
    {
        mout = m;
    }

    void QueryTimeStatsPrinter::nextFile()
    {
        ++files;
        fStartTimePoint = std::chrono::system_clock::now();
    }

    void QueryTimeStatsPrinter::start()
    {
        startTimePoint = std::chrono::system_clock::now();
    }

    void QueryTimeStatsPrinter::nextSentence()
    {
        ++sentences;
    }

    void QueryTimeStatsPrinter::done()
    {
        printTimeStats(false, true);
        std::cout << std::endl;

        sentences += fSentences;
        counter += fCounter;
        oov += fOOV;

        fSentences = 0;
        fCounter = 0;
        fOOV = 0;

        currentTimePoint = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = currentTimePoint-startTimePoint;

        double avgPerSecond = counter*1.0/elapsedSeconds.count();

        *mout << std::fixed << "\r"
              << "\tPattern: " << std::setw(10) << counter
              << " (" << std::setw(4) << (oov*1.0/counter*100) << "% OOV) "
              << std::setw(8) << ((int) avgPerSecond) << "P/s"
              << " took " << elapsedSeconds.count() << " seconds";
        *mout << std::endl;
    }

    void QueryTimeStatsPrinter::printTimeStats(bool isOOV, bool done)
    {
        if(!done)
        {
            ++fCounter;
            if(isOOV) ++fOOV;
        }

        if(fCounter < 50000 || fCounter % 50000 == 0 || done)
        {
            currentTimePoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedSeconds = currentTimePoint-fStartTimePoint;

            double avgPerSecond = fCounter*1.0/elapsedSeconds.count();

            *mout << std::fixed << "\r"
                      << "\tPattern: " << std::setw(10) << fCounter
                      << " (" << std::setw(4) << (fOOV*1.0/fCounter*100) << "% OOV) "
                      << std::setw(8) << ((int) avgPerSecond) << "P/s"
                      << " took " << elapsedSeconds.count() << " seconds";
            *mout << std::flush;
        }
    }

    TimeStatsPrinter::TimeStatsPrinter(unsigned long long totalCount, my_ostream* m)
    {
        total = totalCount;
        mout = m;
    }

    void TimeStatsPrinter::start()
    {
        startTimePoint = std::chrono::system_clock::now();
    }

    void TimeStatsPrinter::reset(bool alsoTimer, int newSample, unsigned long long newValue)
    {
        counter = newValue;
        sample = newSample;
        if(alsoTimer)
        {
            startTimePoint = std::chrono::system_clock::now();
        }
    }

    void TimeStatsPrinter::nextSample()
    {
        reset(true, sample+1, 0);
        std::cout << std::endl;
    }

    void TimeStatsPrinter::increment()
    {
        ++counter;
    }

    void TimeStatsPrinter::printTimeStats()
    {
        ++counter;

        if(counter == total)
        {
            currentTimePoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedSeconds = currentTimePoint-startTimePoint;

            double avgPerSecond = counter*1.0/elapsedSeconds.count();

            *mout << std::fixed << "\r"
                  << "Sample [" << sample << "]"
                  << "\tPattern: " << std::setw(10) << counter
                  <<  " (" << std::setw(4) << (counter*1.0/total*100) << "%) "
                  << std::setw(8) << ((int) avgPerSecond) << "P/s"
                  << " took " << elapsedSeconds.count() << " seconds";
            *mout << std::endl;
        } else if(counter < 20000 || counter % 20000 == 0)
        {
            currentTimePoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedSeconds = currentTimePoint-startTimePoint;

            double avgPerSecond = counter*1.0/elapsedSeconds.count();

            std::cout << std::fixed << "\r"
                      << "Sample [" << sample << "]"
                      << "\tPattern: " << std::setw(10) << counter
                      <<  " (" << std::setw(4) << (counter*1.0/total*100) << "%) "
                      << std::setw(8) << ((int) avgPerSecond) << "P/s"
                      << " seconds remaining:" << (total-counter)*1.0/avgPerSecond;
            std::cout << std::flush;
        }
    }

