### Generate programs
#g++ -std=c++0x -DkORDER='2' -g -O3 -I.. -o thg2 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I/home/lonrust/local/include/colibri-core -I../tools
#g++ -std=c++0x -DkORDER='3' -g -O3 -I.. -o thg3 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I/home/lonrust/local/include/colibri-core -I../tools
#g++ -std=c++0x -DkORDER='4' -g -O3 -I.. -o thg4 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I/home/lonrust/local/include/colibri-core -I../tools
#g++ -std=c++0x -DkORDER='5' -g -O3 -I.. -o thg5 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I/home/lonrust/local/include/colibri-core -I../tools

g++ -std=c++0x -DkORDER='2' -g -O3 -I.. -I../../colibri-core/include -o thg2 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I../tools
g++ -std=c++0x -DkORDER='3' -g -O3 -I.. -I../../colibri-core/include -o thg3 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I../tools
g++ -std=c++0x -DkORDER='4' -g -O3 -I.. -I../../colibri-core/include -o thg4 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I../tools
g++ -std=c++0x -DkORDER='5' -g -O3 -I.. -I../../colibri-core/include -o thg5 -lcolibricore -lboost_filesystem -lboost_system -lboost_serialization -lbz2 test_hpyplm_generatepatterns.cc -I../tools

### Generate files
./thg2 -f testdocs/apology.txt -o . -m apology 2> /dev/null
./thg3 -f testdocs/apology.txt -o . -m apology 2> /dev/null
./thg4 -f testdocs/apology.txt -o . -m apology 2> /dev/null
./thg5 -f testdocs/apology.txt -o . -m apology 2> /dev/null
./thg2 -f testdocs/apology.txt -o . -S -m apology 2> /dev/null
./thg3 -f testdocs/apology.txt -o . -S -m apology 2> /dev/null
./thg4 -f testdocs/apology.txt -o . -S -m apology 2> /dev/null
./thg5 -f testdocs/apology.txt -o . -S -m apology 2> /dev/null

## Count number of unigrams
T1_1_1reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_1_1result="$(cut -f2 apology_2_W1_t1_T1_p0_train.cls | wc -l)"
T1_2_1reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_2_1result="$(cut -f2 apology_3_W1_t1_T1_p0_train.cls | wc -l)"
T1_3_1reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_3_1result="$(cut -f2 apology_4_W1_t1_T1_p0_train.cls | wc -l)"
T1_4_1reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_4_1result="$(cut -f2 apology_5_W1_t1_T1_p0_train.cls | wc -l)"
T1_1_2reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_1_2result="$(cut -f2 apology_2S_W1_t1_T1_p0_train.cls | wc -l)"
T1_2_2reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_2_2result="$(cut -f2 apology_3S_W1_t1_T1_p0_train.cls | wc -l)"
T1_3_2reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_3_2result="$(cut -f2 apology_4S_W1_t1_T1_p0_train.cls | wc -l)"
T1_4_2reference="$(cat testdocs/apology.txt | tr -s ' ' '\n' | sort | uniq | wc -l)"
T1_4_2result="$(cut -f2 apology_5S_W1_t1_T1_p0_train.cls | wc -l)"


if [[ $T1_1_1reference == $T1_1_1result ]]; then echo -n "wel T1_1_1 "; else echo -n "niet T1_1_1 " ;fi
if [[ $T1_2_1reference == $T1_2_1result ]]; then echo -n "wel T1_2_1 "; else echo -n "niet T1_2_1 " ;fi
if [[ $T1_3_1reference == $T1_3_1result ]]; then echo -n "wel T1_3_1 "; else echo -n "niet T1_3_1 " ;fi
if [[ $T1_4_1reference == $T1_4_1result ]]; then echo -n "wel T1_4_1 "; else echo -n "niet T1_4_1 " ;fi
echo
if [[ $T1_1_2reference == $T1_1_2result ]]; then echo -n "wel T1_1_2 "; else echo -n "niet T1_1_2 " ;fi
if [[ $T1_2_2reference == $T1_2_2result ]]; then echo -n "wel T1_2_2 "; else echo -n "niet T1_2_2 " ;fi
if [[ $T1_3_2reference == $T1_3_2result ]]; then echo -n "wel T1_3_2 "; else echo -n "niet T1_3_2 " ;fi
if [[ $T1_4_2reference == $T1_4_2result ]]; then echo -n "wel T1_4_2 "; else echo -n "niet T1_4_2 " ;fi
echo

### Count number of n-grams
T2_1_1reference=42
T2_1_1result="$(colibri-patternmodeller -i apology_2_W1_t1_T1_p0_train.patternmodel -f apology_3_W1_t1_T1_p0_train.dat -c apology_3_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"
T2_2_1reference=42
T2_2_1result="$(colibri-patternmodeller -i apology_3_W1_t1_T1_p0_train.patternmodel -f apology_3_W1_t1_T1_p0_train.dat -c apology_3_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"
T2_3_1reference=42
T2_3_1result="$(colibri-patternmodeller -i apology_4_W1_t1_T1_p0_train.patternmodel -f apology_3_W1_t1_T1_p0_train.dat -c apology_3_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"
T2_4_1reference=42
T2_4_1result="$(colibri-patternmodeller -i apology_5_W1_t1_T1_p0_train.patternmodel -f apology_3_W1_t1_T1_p0_train.dat -c apology_3_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"
T2_1_2reference=42
T2_1_2result="$(colibri-patternmodeller -i apology_2S_W1_t1_T1_p0_train.patternmodel -f apology_2S_W1_t1_T1_p0_train.dat -c apology_2S_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"
T2_2_2reference=42
T2_2_2result="$(colibri-patternmodeller -i apology_3S_W1_t1_T1_p0_train.patternmodel -f apology_3S_W1_t1_T1_p0_train.dat -c apology_3S_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"
T2_3_2reference=42
T2_3_2result="$(colibri-patternmodeller -i apology_4S_W1_t1_T1_p0_train.patternmodel -f apology_4S_W1_t1_T1_p0_train.dat -c apology_4S_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"
T2_4_2reference=42
T2_4_2result="$(colibri-patternmodeller -i apology_5S_W1_t1_T1_p0_train.patternmodel -f apology_5S_W1_t1_T1_p0_train.dat -c apology_5S_W1_t1_T1_p0_train.cls -t 1 -m 1 -l 5 -P 2> /dev/null | tail -n+2 | wc -l)"

if [[ $T2_1_1reference == $T2_1_1result ]]; then echo -n "wel T2_1_1 "; else echo -n "niet T2_1_1 " ;fi
if [[ $T2_2_1reference == $T2_2_1result ]]; then echo -n "wel T2_2_1 "; else echo -n "niet T2_2_1 " ;fi
if [[ $T2_3_1reference == $T2_3_1result ]]; then echo -n "wel T2_3_1 "; else echo -n "niet T2_3_1 " ;fi
if [[ $T2_4_1reference == $T2_4_1result ]]; then echo -n "wel T2_4_1 "; else echo -n "niet T2_4_1 " ;fi
echo
if [[ $T2_1_2reference == $T2_1_2result ]]; then echo -n "wel T2_1_2 "; else echo -n "niet T2_1_2 " ;fi
if [[ $T2_2_2reference == $T2_2_2result ]]; then echo -n "wel T2_2_2 "; else echo -n "niet T2_2_2 " ;fi
if [[ $T2_3_2reference == $T2_3_2result ]]; then echo -n "wel T2_3_2 "; else echo -n "niet T2_3_2 " ;fi
if [[ $T2_4_2reference == $T2_4_2result ]]; then echo -n "wel T2_4_2 "; else echo -n "niet T2_4_2 " ;fi
echo

### Test output directory
T3_1dir="$(mktemp -d)"
./thg3 -f testdocs/apology.txt -o "$T3_1dir" -m apology 2> /dev/null
T3_1reference=3
T3_1result="$(ls $T3_1dir | wc -l)"
if [[ $T3_1reference == $T3_1result ]]; then echo -n "wel T3_1 "; else echo -n "niet T3_1 " ;fi
rm -rf "$T3_1dir"

### Test input directory
T4_1reference="$(cat testdocs/* | tr -s ' ' '\n' | sort | uniq | wc -l)"
./thg3 -i testdocs -o . -m testdocs 2> /dev/null
T4_1result="$(wc -l testdocs_3_W1_t1_T1_p0_train.cls | cut -d' ' -f1)"
if [[ $T4_1reference == $T4_1result ]]; then echo -n "wel T4_1 "; else echo -n "niet T4_1 " ;fi
echo


### Test is probs sum to one for context...
