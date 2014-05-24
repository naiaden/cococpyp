#all: hpyplm dhpyplm
all: hpyplm3 #hpyplm4 hpyplm5 hpyplm6 hpyplm7 shpyplm3 shpyplm4 shpyplm5

COCO_INCLUDE=/usr/local/include/colibri-core
#COCO_INCLUDE=/home/lonrust/Software/colibri-core/include
#COCO_LIB=/home/louis/Software/colibri-core/src
BOOST_INCLUDE=/usr/include/boost
BOOST_SERIALIZATION=$(BOOST_ROOT)/lib/libboost_serialization.a

hpyplm3: hpyplm.cc
		$(CXX) -std=c++11 -g -DkORDER='3' -O0 -Wall -I.. hpyplm.cc -o hpyplm3 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE)
#
#hpyplm4: hpyplm.cc
#	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -Wall -I.. hpyplm.cc -o hpyplm4 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lbz2
#
#hpyplm5: hpyplm.cc
#	$(CXX) -std=c++11 -DkORDER='5' -g -O0 -Wall -I.. hpyplm.cc -o hpyplm5 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lbz2
#
#hpyplm6: hpyplm.cc
#	$(CXX) -std=c++11 -DkORDER='6' -g -O0 -Wall -I.. hpyplm.cc -o hpyplm6 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lbz2
#
#hpyplm7: hpyplm.cc
#	$(CXX) -std=c++11 -DkORDER='7' -g -O0 -Wall -I.. hpyplm.cc -o hpyplm7 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lbz2
#
#shpyplm3: shpyplm.cc
#	$(CXX) -std=c++11 -DkORDER='3' -g -O0 -Wall -I.. shpyplm.cc -o shpyplm3 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lbz2
#
#shpyplm4: shpyplm.cc
#	$(CXX) -std=c++11 -DkORDER='4' -g -O0 -Wall -I.. shpyplm.cc -o shpyplm4 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lbz2
#
#shpyplm5: shpyplm.cc
#	$(CXX) -std=c++11 -DkORDER='5' -g -O0 -Wall -I.. shpyplm.cc -o shpyplm5 -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lbz2

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

