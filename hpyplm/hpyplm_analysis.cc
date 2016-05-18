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

#include "utils.h"
#include "date.h"
#include "strategies.h"

using date::operator<<;

int main(int argc, char** argv) {
    std::cout << "Started at " << std::chrono::system_clock::now()  << std::endl;
    
    std::stringstream oss;
    oss << kORDER;
    std::string _kORDER = oss.str();

    AnalysisCommandLineOptions aclo = AnalysisCommandLineOptions(argc, argv);
    std::cout << "Loaded ACLO" << std::endl;
    AnalysisProgramOptions po = AnalysisProgramOptions(aclo);
    std::cout << "Loaded PO" << std::endl;
    PatternModelOptions pmo = DefaultPatternModelOptions(false, kORDER).patternModelOptions;
    std::cout << "Loaded PMO" << std::endl;

    AnalysisCoCoInitialiser cci(po, pmo, true);
    std::cout << "Loaded CCI" << std::endl;


    std::string moutputFile(po.generalBaseOutputName + ".output");
    my_ostream mout(moutputFile);

    mout << "Initialisation done at " << std::chrono::system_clock::now() << std::endl;


    std::ifstream ifs(po.trainSerialisedFileName, std::ios::binary);
    if(!ifs.good()) {
        std::cerr << "Something went wrong whilst reading the model: " << po.trainSerialisedFileName << std::endl;
    }
    boost::archive::binary_iarchive ia(ifs);

    cpyp::PYPLM<kORDER> lm;
    ia & lm;
    mout << "Loaded serialised model" << std::endl;

/////////////////////////////////////////////////

    PatternSet<uint64_t> allWords = cci.trainPatternModel.extractset(1,1);
    mout << "Extracted all words" << std::endl;

    mout << "Preparation done at " << std::chrono::system_clock::now() << std::endl;

    int total_num_tables = 0;
	int total_num_customers = 0;

	int c_size = 0;

	for(auto kv : lm.p) { // p = p_


		std::string p_to_string = kv.first.tostring(cci.classDecoder);

		if(p_to_string.size() == 0)
			c_size = 0;
		else
			c_size = std::count( p_to_string.begin(), p_to_string.end(), ' ' ) + 1;

		 std::vector<std::string> focus_words = kv.second.give_focus_words(&(cci.classDecoder));

	}
//
//
//		if(c_size == _only_context_size)
//		{
//
//			int num_tables = kv.second.num_tables();
//			//total_num_tables += num_tables;
//			int num_customers = kv.second.num_customers();
//			//total_num_customers += num_customers;
//
//
//			p2bo(((c_size ? (p_to_string + " ") : "")) + "\t" + std::to_string(num_customers) + "\n", _ngram_output);
//		}
//
///*        if(c_size == _only_context_size)
//		{
//			int num_tables = kv.second.num_tables();
//			//total_num_tables += num_tables;
//			int num_customers = kv.second.num_customers();
//			//total_num_customers += num_customers;
//			 for(auto s : focus_words)
//			 {
//				p2bo(((c_size ? (p_to_string + " ") : "") + s) + "\n", _ngram_output);
//				//p2bo(((c_size ? (p_to_string + " ") : "") + s) + "\t" + std::to_string(num_customers) + "\n", _ngram_output);
//			 }
//		}
//*/
//	}

}



