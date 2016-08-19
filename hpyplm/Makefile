all: hpyplm3 hpyplm3 hpyplm4 hpyplm5 hpyplm_train3 hpyplm_train4 hpyplm_query3 hpyplm_query4 loadmodel

COCO_INCLUDE=/home/louis/local/include/colibri-core
COCO_LIB=/home/louis/local/lib

hpyplm2: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='2' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm3: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm4: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm5: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='5' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2



hpyplm_train3: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

hpyplm_train4: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

hpyplm_train5: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='5' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)



hpyplm_query3: hpyplm_query_sncbw.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

hpyplm_query4: hpyplm_query_sncbw.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

hpyplm_query5: hpyplm_query_sncbw.cc
	$(CXX) -std=c++11 -DkORDER='5' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)



hpyplm_loadmodel: hpyplm_loadmodel.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -L$(COCO_LIB)

