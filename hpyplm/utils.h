#ifndef UTILS_H_
#define UTILS_H_

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

enum class Backoff { NGRAM, LIMITED, FULL, ENTINT, FREQINT, ALL };

inline Backoff fromString(const std::string& s) {
    if(s.compare("ngram") == 0) return Backoff::NGRAM;
    else if(s.compare("limited") == 0) return Backoff::LIMITED;
    else if(s.compare("full") == 0) return Backoff::FULL;
    else if(s.compare("entint") == 0) return Backoff::ENTINT;
    else if(s.compare("freqint") == 0) return Backoff::FREQINT;

    else if(s.compare("all") == 0) return Backoff::ALL;

    else return Backoff::NGRAM;
}

inline std::string toString(Backoff b) {
    if(b == Backoff::NGRAM) return "ngram";
    if(b == Backoff::LIMITED) return "limited";
    if(b == Backoff::FULL) return "full";
    if(b == Backoff::ENTINT) return "entint";
    if(b == Backoff::FREQINT) return "freqint";
    if(b == Backoff::ALL) return "all";
    return "unknown backoff method";
}


inline std::vector<std::string> split(std::string const &input) {
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer),
                                 std::istream_iterator<std::string>()};
    return ret;
}

class my_ostream // http://ideone.com/T5Cy1M
{
    public:
        my_ostream(std::string fileName) : my_fstream(fileName) {}; // check if opening file succeeded!!
        // for regular output of variables and stuff
        template<typename T> my_ostream& operator<<(const T& something)
        {
            std::cout << something;
            my_fstream << something;
            return *this;
        }
        // for manipulators like std::endl
        typedef std::ostream& (*stream_function)(std::ostream&);
        my_ostream& operator<<(stream_function func)
        {
            func(std::cout);
            func(my_fstream);
            return *this;
        }
    private:
        std::ofstream my_fstream;
};

struct PatternComp
{
    bool operator() (const Pattern& lhs, const Pattern& rhs) const
    {
        return lhs>rhs;
    }
};


 


class QueryTimeStatsPrinter
{
    my_ostream* mout;

    int files = 0;

    unsigned long sentences = 0;
    unsigned long long counter = 0;
    unsigned long long oov = 0;

    unsigned long long fSentences = 0;
    unsigned long long fCounter = 0;
    unsigned long long fOOV = 0;

    std::chrono::time_point<std::chrono::system_clock> startTimePoint, currentTimePoint, fStartTimePoint;
public:
    QueryTimeStatsPrinter(my_ostream* m)
    {
        mout = m;
    }

    void nextFile()
    {
        ++files;
        fStartTimePoint = std::chrono::system_clock::now();
    }

    void start()
    {
        startTimePoint = std::chrono::system_clock::now();
    }

    void nextSentence()
    {
        ++sentences;
    }

    void done()
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

    void printTimeStats(bool isOOV = false, bool done = false)
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
};

class TimeStatsPrinter
{
    my_ostream* mout;
    
    unsigned long long counter = 0;
    int sample = 0;
    unsigned long long total = 0;
    std::chrono::time_point<std::chrono::system_clock> startTimePoint, currentTimePoint;

public:
    TimeStatsPrinter(unsigned long long totalCount, my_ostream* m)
    {
        total = totalCount;
        mout = m;
    }

    void start()
    {
        startTimePoint = std::chrono::system_clock::now();
    }

    void reset(bool alsoTimer = true, int newSample = 0, unsigned long long newValue = 0) 
    {
        counter = newValue;
        sample = newSample;
        if(alsoTimer)
        {
            startTimePoint = std::chrono::system_clock::now();
        }
    }

    void nextSample()
    {
        reset(true, sample+1, 0);
        std::cout << std::endl;
    }

    void increment()
    {
        ++counter;
    }

    void printTimeStats()
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
};


#endif
