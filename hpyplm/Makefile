all: ht4 hq4

COCO_INCLUDE=$(HOME)/local/include/colibri-core/
BOOST_INCLUDE=/usr/include/boost
BOOST_SERIALIZATION=$(BOOST_ROOT)/lib/libboost_serialization.a

COCO_LIB=/home/louis/local/lib



ht4: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< Debug.cpp utils.cpp strategies.cpp LimitedCounts.cpp ContextValues.cpp ContextCounts.cpp PLNCache.cpp PatternCounts.cpp ProgramOptions.cpp PatternCache.cpp -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L/users/spraak/onrust/software/boost_1_55_0/stage/lib/

hq4: hpyplm_query_sncbw.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -I.. $< LimitedTracker.cpp Debug.cpp utils.cpp strategies.cpp LimitedCounts.cpp ContextValues.cpp ContextCounts.cpp PLNCache.cpp PatternCounts.cpp ProgramOptions.cpp PatternCache.cpp -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L/users/spraak/onrust/software/boost_1_55_0/stage/lib/


hpyplm_loadmodel: hpyplm_loadmodel.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

hqn4: hpyplm_query_nbest.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< NbestList.cpp Debug.cpp utils.cpp strategies.cpp LimitedCounts.cpp ContextValues.cpp ContextCounts.cpp PLNCache.cpp PatternCounts.cpp ProgramOptions.cpp PatternCache.cpp -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L/users/spraak/onrust/software/boost_1_55_0/stage/lib/
