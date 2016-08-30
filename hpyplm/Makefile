all: ht4 hq4

COCO_INCLUDE=/home/louis/local/include/colibri-core
COCO_LIB=/home/louis/local/lib


ht4: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

hq4: hpyplm_query_sncbw.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

hpyplm_loadmodel: hpyplm_loadmodel.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

