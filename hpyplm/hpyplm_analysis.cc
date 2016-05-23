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

    long int total_num_tables = 0;
    long int total_num_customers = 0;

    int c_size = 0;

    for(auto kv : lm.p) { // p = p_
        total_num_tables += kv.second.num_tables();
        total_num_customers += kv.second.num_customers();
    }

    std::cout << "tables: " << total_num_tables << std::endl;
    std::cout << "customers: " << total_num_customers << std::endl;
}



