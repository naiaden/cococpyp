#all: hpyplm dhpyplm
all: hpyplm3 hpyplm3 hpyplm4 hpyplm5 hpyplm_train3 hpyplm_train4 hpyplm_query3 hpyplm_query4 loadmodel
#hpyplm4 hpyplm5 hpyplm6 hpyplm7 hpyplm8 hpyplm9 hpyplm10
#hpyplm6 hpyplm7 shpyplm3 shpyplm4 shpyplm5

#COCO_INCLUDE=/usr/local/include/colibri-core
COCO_INCLUDE=/home/lonrust/Software/colibri-core/include
#COCO_LIB=/home/louis/Software/colibri-core/src
BOOST_INCLUDE=/usr/include/boost
BOOST_SERIALIZATION=$(BOOST_ROOT)/lib/libboost_serialization.a

hpyplm2: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='2' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm3: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm4: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm5: hpyplm.cc
	$(CXX) -std=c++11 -DkORDER='5' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

#dhpyplm: dhpyplm.cc
#	$(CXX) -std=c++11 -O3 -g -Wall -I.. dhpyplm.cc -o dhpyplm

## stuff below here is optional

hpyplm_train: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_train3: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_traino4: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_train4: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_train5: hpyplm_train.cc
	$(CXX) -std=c++11 -DkORDER='5' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_query: hpyplm_query.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2
#	//$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

hpyplm_query3: hpyplm_query.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_query4: hpyplm_query.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_query_sncbw: hpyplm_query_sncbw.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_backoffcountquery4: hpyplm_backoffcountquery.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_query5: hpyplm_query.cc
	$(CXX) -std=c++11 -DkORDER='5' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_query_observe: hpyplm_query_observe.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

hpyplm_analysis3: hpyplm_analysis.cc
	$(CXX) -std=c++11 -DkORDER='3' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_analysis4: hpyplm_analysis.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

hpyplm_loadmodel: hpyplm_loadmodel.cc
	$(CXX) -std=c++11 -DkORDER='4' -g -O3 -I.. $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2

dhpyplm_train: dhpyplm_train.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

dhpyplm_query: dhpyplm_query.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

CDEC = ../../cdec
cdec_ff_hpyplm.o: cdec_ff_hpyplm.cc
	$(CXX) -shared -fPIC -std=c++11 -O3 -g -Wall -I$(BOOST_INCLUDE) -I.. -I$(CDEC)/utils -I$(CDEC)/mteval -I$(CDEC)/decoder $< -c

libcdec_ff_hpyplm.so: cdec_ff_hpyplm.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $<

