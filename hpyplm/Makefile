#all: hpyplm dhpyplm
all: hpyplm

COCO_INCLUDE=/usr/local/include/colibri-core
#COCO_INCLUDE=/home/louis/Software/colibri-core/include
#COCO_LIB=/home/louis/Software/colibri-core/src
BOOST_INCLUDE=/usr/include/boost
BOOST_SERIALIZATION=$(BOOST_ROOT)/lib/libboost_serialization.a

hpyplm: hpyplm.cc
	$(CXX) -std=c++11 -g -O0 -Wall -I.. hpyplm.cc -o hpyplm -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE)
#	$(CXX) -std=c++11 -DDEBUG -O0 -g3 -Wall -I.. hpyplm.cc -o hpyplm -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -I"" -c -fmessage-length=0 -MMD -MP	

#dhpyplm: dhpyplm.cc
#	$(CXX) -std=c++11 -O3 -g -Wall -I.. dhpyplm.cc -o dhpyplm

## stuff below here is optional

hpyplm_train: hpyplm_train.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

hpyplm_query: hpyplm_query.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

hpyplm_query_observe: hpyplm_query_observe.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)


dhpyplm_train: dhpyplm_train.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

dhpyplm_query: dhpyplm_query.cc
	$(CXX) -std=c++11 -O3 -Wall -I$(BOOST_INCLUDE) -I.. $< -o $@ $(BOOST_SERIALIZATION)

CDEC = ../../cdec
cdec_ff_hpyplm.o: cdec_ff_hpyplm.cc
	$(CXX) -shared -fPIC -std=c++11 -O3 -g -Wall -I$(BOOST_INCLUDE) -I.. -I$(CDEC)/utils -I$(CDEC)/mteval -I$(CDEC)/decoder $< -c

libcdec_ff_hpyplm.so: cdec_ff_hpyplm.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $<

