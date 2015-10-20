# Makefile template for shared library

### Options
AMSROOTLIBS   :=$(AMSWD)/include # amschain.h
AMSSTATICLIBS := -L$(AMSWD)/lib/linuxx8664gcc5.34 -Llib -lntuple_slc6_PG_dynamic
INCLUDES      := -Iinclude
ROOT_INCLUDES := -I$(AMSROOTLIBS)
DEBUG	      := -g

#Ca va chier dans le compilo
ROOTCFG  := root-config
#CXX      :=$(shell $(ROOTCFG) --cxx)   #g++
CXX	:= g++
CXXFLAGS :=-std=c++11 $(INCLUDES) $(ROOT_INCLUDES) $(DEBUG) $(shell $(ROOTCFG) --cflags) -D_PGTRACK_ -Wno-write-strings -fPIC # TH1F... 
ROOTLIBS :=-L/$(shell $(ROOTCFG) --libdir --libs) 

#$?  List of dependencies changed more recently than current target.
#$^  List of dependencies.
#$@  Name of current target.
#$<  Name of current dependency.
#$*  Name of current dependency without extension.


SRCS = generalUtils.cpp rootUtils.cpp Stack.cpp GraphFromHistos.cpp Loop.cpp DstAmsBinary.cpp  # source files
OBJS = $(SRCS:.cpp=.o)

all: lib/libGeneralUtils.so  lib/libRootUtils.so lib/libStack.so lib/libDstAmsBinary.so  # bin/ReduceSample bin/makeChain

lib/libGeneralUtils.so: generalUtils.o
	$(CXX) $(INCLUDES) -shared -o $@ $^

lib/libRootUtils.so: rootUtils.o
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^

lib/libStack.so: Stack.o
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^ 

lib/libGraphFromHistos.so: GraphFromHistos.o
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^

lib/libDstAmsBinary.so: DstAmsBinary.o Loop.o
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^

$(SRCS:.cpp=.o):%.o:src/%.cpp include/%.hpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/ReduceSample: src/ReduceSample.cpp lib/libRootUtils.so
	$(CXX) $(CXXFLAGS)-o $@ $< ${ROOTLIBS} ${AMSSTATICLIBS} -lRootUtils

bin/dircount: src/dircount.cpp
	$(CXX) $(CXXFLAGS)-o $@ $<

bin/makeChain: src/makeChain.cpp lib/libRootUtils.so 
	$(CXX) $(CXXFLAGS)-o $@ $< -lRootUtils ${ROOTLIBS} ${AMSSTATICLIBS}

.PHONY: clean
clean:
	rm -f lib/*.so ${OBJS} lib/*.a
