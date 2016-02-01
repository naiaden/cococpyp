#include <iostream>
#include <unordered_map>
#include <cstdlib>

#include "hpyplm.h"

#include "cpyp/boost_serializers.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <map>
#include <set>

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

#include <sstream>
#include <iomanip>

#include <queue>

#include "cmdline.h"
#include "DefaultPatternModelOptions.h"
#include "ProgramOptions.h"
#include "CoCoInitialiser.h"

std::vector<std::string> split(std::string const &input) { 
    std::istringstream buffer(input);
    std::vector<std::string> ret{std::istream_iterator<std::string>(buffer), 
                                 std::istream_iterator<std::string>()};
    return ret;
}

void p2b(const std::string& s, std::ostream& os, std::ofstream& ofs) {

    ofs << s;
    os << s;
}

void p2bo(const std::string& s, std::ofstream& ofs) {
    p2b(s, std::cout, ofs);
}

void p2be(const std::string& s, std::ofstream& ofs) {
    p2b(s, std::cerr, ofs);
}

std::string giveTime()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string _current_time(buffer);

    return _current_time;
}

int main(int argc, char** argv) {
    std::cout << "Started at " << giveTime() << std::endl;
    
    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();

    SNCBWCommandLineOptions qclo = SNCBWCommandLineOptions(argc, argv);
    std::cout << "Loaded QCLO" << std::endl;
    SNCBWProgramOptions po = SNCBWProgramOptions(qclo, std::stoi(_kORDER));
    std::cout << "Loaded PO" << std::endl;
    PatternModelOptions pmo = DefaultPatternModelOptions(false, kORDER).patternModelOptions;
    std::cout << "Loaded PMO" << std::endl;

    CoCoInitialiser cci = CoCoInitialiser(po, pmo, true);
    std::cout << "Loaded CCI" << std::endl;

    std::ofstream generalOutput;
    std::string generalOutputFilename = po.generalBaseOutputName + ".output";
    generalOutput.open(generalOutputFilename);

    std::cout << "Initialisation done at " << giveTime() << std::endl;


    std::ifstream ifs(po.trainSerialisedFileName, std::ios::binary);
    if(!ifs.good()) {
        std::cerr << "Something went wrong whilst reading the model: " << po.trainSerialisedFileName << std::endl;
    }
    boost::archive::binary_iarchive ia(ifs);

    cpyp::PYPLM<kORDER> lm;
    ia & lm;
    std::cout << "Loaded serialised model" << std::endl;


    PatternSet<uint64_t> allPatterns = cci.trainPatternModel.extractset();
    std::cout << "Extracted all patterns" << std::endl;

    std::cout << "Preparation done at " << giveTime() << std::endl;

    generalOutput.close();






} /*


    



    std::vector<Backoff> all_backoff_options = std::vector<Backoff>();

    if(_backoff_method == Backoff::ALL) {
         all_backoff_options.push_back(Backoff::NGRAM);
         all_backoff_options.push_back(Backoff::BOBACO);
         all_backoff_options.push_back(Backoff::GLM);
    } else 
    {
         all_backoff_options.push_back(_backoff_method);
    }

    for(Backoff i_backoff_method : all_backoff_options)
    {
        std::string _base_output_name = _output_directory + "/" + _output_run_name + "_" + toString(i_backoff_method) + "_" + _kORDER;
        std::string _output_class_file_name = _base_output_name + ".cls";
        std::string _output_corpus_file_name = _base_output_name + ".dat";
        std::string _output_patternmodel_file_name = _base_output_name + ".patternmodel";
        //std::string _output_serialised_file_name = _base_output_name + ".ser";
        std::string _output_probabilities_file_name = _base_output_name + ".probs";

        std::ofstream _output;
        std::string _output_filename = _base_output_name + ".output";
        _output.open(_output_filename);

        std::ofstream _probs_file;
        _probs_file.open(_output_probabilities_file_name);

        std::cout << "Processing method " << toString(i_backoff_method) << std::endl;

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
        _current_time = std::string(buffer);

        p2bo("Time: " + _current_time + "\n", _output);


        double llh = 0;
        unsigned cnt = 0;
        unsigned oovs = 0;
        


        int nr_files = 0;
        int nr_lines = 0;
        std::map<int, int> backoff_administration;
        for(std::string input_file_name : test_input_files)
        {
            ++nr_files;
            std::ifstream file(input_file_name);


            std::string retrieved_string;
            while( std::getline(file, retrieved_string))
            {
                ++nr_lines;
                std::vector<std::string> words = split(retrieved_string);

                if(words.size() < kORDER)
                {
                } else
                {
                    // als kORDER = 4, dan is 3 het focuswoord
                    for(int i = (kORDER-1); i < words.size(); ++i)
                    {
                        std::stringstream context_stream;
                        context_stream << words[i-(kORDER-1)];

                        for(int ii = 1; ii < kORDER-1 ; ++ii)
                        {
                            context_stream << " " << words[i-(kORDER-1)+ii];
                        }

                        Pattern context = _class_encoder.buildpattern(context_stream.str());
                        Pattern focus = _class_encoder.buildpattern(words[i]);

                        double lp;

                        // deze moet er wel in natuurlijk
                        bool backoff_to_skips = false;
                        lp = log(lm.prob(focus, context, nullptr, backoff_to_skips, &backoff_administration))    /log(2);

                        if(!allPatterns.has(focus)) {
                            ++oovs;
                            lp = 0;
                            _probs_file << "***";
                        }

                        _probs_file << "p(" << focus.tostring(_class_decoder) << " |";
                        _probs_file << context.tostring(_class_decoder) << ") = " << std::fixed << std::setprecision(20) << lp << std::endl;

                        llh -= lp;
                        ++cnt;

                    }
                }
            }
        }

        for(auto key: backoff_administration)
        {
            std::cout << key.first << " " << key.second << std::endl;
        }

        std::cout << "Processed " << nr_files << " files and " << nr_lines << " lines" << std::endl;

        cnt -= oovs;
        double lprob = (-llh * log(2)) / log(10); // in cpyp: (-llh * log(2) / log(10))
        p2be("  Log_10 prob: " + std::to_string(lprob) + "\n" , _output);
        p2be("        Count: " + std::to_string(cnt) + "\n", _output);
        p2be("         OOVs: " + std::to_string(oovs) + "\n", _output);
        p2be("Cross-Entropy: " + std::to_string((llh / cnt)) + "\n", _output);
        p2be("   Perplexity: " + std::to_string(pow(2, llh / cnt)) + "\n", _output);

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeinfo);
        _current_time = std::string(buffer);

        p2bo("Time: " + _current_time + "\n", _output);
        _probs_file.close();
    }

    p2be("Done for now\n" , _general_output);
    _general_output.close();
    

    return 0;

}
*/
