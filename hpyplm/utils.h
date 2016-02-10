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

std::vector<std::string> split(std::string const &input) {
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

class TimeStatsPrinter
{
    int counter = 0;
    int sample = 0;
    int total = 0;
    std::chrono::time_point<std::chrono::system_clock> startTimePoint, currentTimePoint;

public:
    TimeStatsPrinter(int totalCount)
    {
        total = totalCount;
    }

    void start()
    {
        startTimePoint = std::chrono::system_clock::now();
    }

    void reset(bool alsoTimer = true, int newSample = 0, int newValue = 0) 
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

            std::cout << std::fixed << "\r" 
                      << "Sample [" << sample << "]"
                      << "\tPattern: " << std::setw(10) << counter 
                      <<  " (" << std::setw(4) << (counter*1.0/total*100) << "%) " 
                      << std::setw(8) << ((int) avgPerSecond) << "P/s"
                      << " took " << elapsedSeconds.count() << " seconds"; 
            std::cout << std::flush;
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
