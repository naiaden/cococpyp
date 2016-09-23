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

#include <string>
#include <sstream>
#include <vector>

enum class Backoff { NGRAM, LIM, LIMENT, LIMMLE, LIMUNI, FULL, FULLENT, FULLMLE, FULLUNI, ENT, MLE, UNI, ALL };

void sssplit(const std::string &s, char delim, std::vector<std::string> &elems);


std::vector<std::string> ssplit(const std::string &s, char delim);


std::vector<Backoff> fromString(const std::string& s);

std::string backoffsToString(const std::vector<Backoff>& bs);
bool backoffIn(Backoff b, const std::vector<Backoff> bs);

std::vector<std::string> wsSplit(std::string const &input);

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
    QueryTimeStatsPrinter(my_ostream* m);

    void nextFile();

    void start();

    void nextSentence();

    void done();

    void printTimeStats(bool isOOV = false, bool done = false);
};

class TimeStatsPrinter
{
    my_ostream* mout;
    
    unsigned long long counter = 0;
    int sample = 0;
    unsigned long long total = 0;
    std::chrono::time_point<std::chrono::system_clock> startTimePoint, currentTimePoint;

public:
    TimeStatsPrinter(unsigned long long totalCount, my_ostream* m);

    void start();

    void reset(bool alsoTimer = true, int newSample = 0, unsigned long long newValue = 0) ;

    void nextSample();

    void increment();

    void printTimeStats();
};


#endif
