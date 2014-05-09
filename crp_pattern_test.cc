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

    const char * poem = 
        "To be or not to be , that is the question ;\n"
        "Whether 'tis nobler in the mind to suffer\n"
        "The slings and arrows of outrageous fortune ,\n"
        "Or to take arms against a sea of troubles ,\n"
        "And by opposing , end them . To die , to sleep ;\n"
        "No more ; and by a sleep to say we end\n"
        "The heart-ache and the thousand natural shocks\n"
        "That flesh is heir to — 'tis a consummation\n"
        "Devoutly to be wish'd . To die , to sleep ;\n"
        "To sleep , perchance to dream . Ay , there's the rub ,\n"
        "For in that sleep of death what dreams may come ,\n"
        "When we have shuffled off this mortal coil ,\n"
        "Must give us pause . There's the respect\n"
        "That makes calamity of so long life ,\n"
        "For who would bear the whips and scorns of time,\n"
        "Th'oppressor's wrong , the proud man 's contumely ,\n"
        "The pangs of despised love , the law 's delay ,\n"
        "The insolence of office , and the spurns\n"
        "That patient merit of th' unworthy takes ,\n"
        "When he himself might his quietus make\n"
        "With a bare bodkin ? who would fardels bear ,\n"
        "To grunt and sweat under a weary life ,\n"
        "But that the dread of something after death ,\n"
        "The undiscovered country from whose bourn\n"
        "No traveller returns , puzzles the will ,\n"
        "And makes us rather bear those ills we have\n"
        "Than fly to others that we know not of ?\n"
        "Thus conscience does make cowards of us all ,\n"
        "And thus the native hue of resolution\n"
        "Is sicklied o'er with the pale cast of thought ,\n"
        "And enterprises of great pitch and moment\n"
        "With this regard their currents turn awry ,\n"
        "And lose the name of action .\n"
        "Soft you now ! The fair Ophelia ! Nymph ,\n"
        "in thy orisons be all my sins remember'd .\n"
        "To flee or not to flee .\n" //additions to test skipgrams
        "To flee or not to flee .\n"
        "To see or not to see .\n"
        "To see or not to see .\n"
        "To pee or not to pee .\n"; 

    ClassDecoder classdecoder = ClassDecoder();
    ClassEncoder classencoder = ClassEncoder();

    PatternModelOptions patternmodeloptions = PatternModelOptions();
    patternmodeloptions.MAXLENGTH = 10;
    patternmodeloptions.MINTOKENS = 1;
    patternmodeloptions.DOREVERSEINDEX = false;

    classencoder.build("/home/louis/data/tiny_vac/test.txt");
    classencoder.save("/tmp/tmpout/crpattern.cls");

    classencoder.encodefile("/home/louis/data/tiny_vac/test.txt", "/tmp/tmpout/crpattern.dat", false, false, true, true);
    classdecoder.load("/tmp/tmpout/crpattern.cls");

    IndexedCorpus indexedcorpus = IndexedCorpus("/tmp/tmpout/crpattern.dat");

    PatternModel<uint32_t> patternmodel = PatternModel<uint32_t>(&indexedcorpus);
    patternmodel.train("/tmp/tmpout/crpattern.dat", patternmodeloptions);

    //for(IndexPattern it : indexedcorpus) {
    //    std::vector<Pattern> result = patternmodel.getreverseindex(it.ref);
    //    for(Pattern p : result) {
    //        std::cout << p.size() << std::endl;
    //    }
    //}

    cpyp::MT19937 eng;
    double tot = 0;
    double xt = 0;
    cpyp::crp<Pattern> crp(0.5, 1.0);
    IndexPattern it = *(indexedcorpus.begin());
    std::vector<Pattern> pattern_dishes = patternmodel.getreverseindex(it.ref);//std::vector<Pattern>();
    for (unsigned i = 0; i < 10; ++i) { Pattern p = Pattern(); pattern_dishes.push_back(p); }
    
    //unsigned cust = 10;
    vector<int> hist(/*cust*/ pattern_dishes.size() + 1, 0);
    for (unsigned i = 0; i < pattern_dishes.size(); ++i) { crp.increment<float, cpyp::MT19937>(pattern_dishes[i], 1.0, eng); }
    const int samples = 20;
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

