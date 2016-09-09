all: ht4 hq4

COCO_INCLUDE=/home/lonrust/Software/colibri-core/include
BOOST_INCLUDE=/usr/include/boost
BOOST_SERIALIZATION=$(BOOST_ROOT)/lib/libboost_serialization.a


ht4: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< strategies.cpp LimitedCounts.cpp ContextValues.cpp ContextCounts.cpp PLNCache.cpp PatternCounts.cpp ProgramOptions.cpp PatternCache.cpp -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hq4: hpyplm_query_sncbw.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< strategies.cpp LimitedCounts.cpp ContextValues.cpp ContextCounts.cpp PLNCache.cpp PatternCounts.cpp ProgramOptions.cpp PatternCache.cpp -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

