#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <vector>

#include "cpyp/crp.h"
#include "cpyp/mf_crp.h"
#include "cpyp/random.h"

#include <classencoder.h>
#include <classdecoder.h>
#include <patternmodel.h>

using namespace std;

int main() {
    cpyp::MT19937 eng;
    double tot = 0;
    double xt = 0;
    cpyp::crp<Pattern> crp(0.5, 1.0);
    std::vector<Pattern> pattern_dishes = std::vector<Pattern>();
    for (unsigned i = 0; i < 10; ++i) { Pattern p = Pattern(); pattern_dishes.push_back(p); }
    
    //unsigned cust = 10;
    vector<int> hist(/*cust*/ pattern_dishes.size() + 1, 0);
    for (unsigned i = 0; i < pattern_dishes.size(); ++i) { crp.increment<float, cpyp::MT19937>(pattern_dishes[i], 1.0, eng); }
    const int samples = 200000;
    for (int k = 0; k < samples; ++k) {
        unsigned da = cpyp::sample_uniform01<double>(eng) * pattern_dishes.size();
        for (unsigned i = 0; i < da; ++i) { crp.decrement<cpyp::MT19937>(pattern_dishes[i], eng); }
        for (unsigned i = 0; i < da; ++i) { crp.increment<float, cpyp::MT19937>(pattern_dishes[i], 1.0, eng); }
        int c = crp.num_tables();
        ++hist[c];
        tot += c;
    }
    assert(pattern_dishes.size() == crp.num_customers());
   // cerr << crp << endl;
    cerr << crp.log_likelihood() << endl;
    cerr << "P(a) = " << (xt / samples) << endl;
    cerr << "mean num tables = " << (tot / samples) << endl;
    double error = fabs((tot / samples) - 5.4);
    cerr << "  error = " << error << endl;
    for (unsigned i = 1; i <= pattern_dishes.size(); ++i)
    cerr << i << ' ' << (hist[i]) << endl;
    if (error > 0.1) {
        cerr << "*** error is too big = " << error << endl;
        return 1;
    }
    return 0;
}

