# Makefile template for shared library

### Options
AMSROOTLIBS   :=$(AMSWD)/include # amschain.h
AMSSTATICLIBS := -L$(AMSWD)/lib/linuxx8664icc5.34 -Llib -lntuple_slc6_PG
INCLUDES      := -Iinclude
ROOT_INCLUDES := -I$(AMSROOTLIBS)
DEBUG	      := -O3

#Ca va chier dans le compilo
ROOTCFG  := root-config
#CXX      :=$(shell $(ROOTCFG) --cxx)   #g++
CXX	:= gcc
CXXFLAGS :=-std=c++11 $(INCLUDES) $(ROOT_INCLUDES) $(DEBUG) -O2 $(shell $(ROOTCFG) --cflags) -D_PGTRACK_ -Wno-write-strings -fPIC # TH1F... 
ROOTLIBS :=-L/$(shell $(ROOTCFG) --libdir --libs) 

#$?  List of dependencies changed more recently than current target.
#$^  List of dependencies.
#$@  Name of current target.
#$<  Name of current dependency.
#$*  Name of current dependency without extension.


SRCS = generalUtils.cpp rootUtils.cpp Stack.cpp GraphFromHistos.cpp Loop.cpp  # source files
OBJS = $(SRCS:.cpp=.o)

all: lib/libGeneralUtils.so bin/ReduceSample bin/makeChain lib/libRootUtils.so lib/libStack.so

lib/libGeneralUtils.so: generalUtils.o
	$(CXX) $(INCLUDES) -shared -o $@ $^

lib/libRootUtils.so: rootUtils.o
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^

lib/libStack.so: Stack.o
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^ 

lib/libGraphFromHistos.so: GraphFromHistos.o
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^

$(SRCS:.cpp=.o):%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/ReduceSample: src/ReduceSample.cpp lib/libRootUtils.so
	$(CXX) $(CXXFLAGS)-o $@ $< -lRootUtils ${AMSSTATICLIBS} ${ROOTLIBS}

bin/dircount: src/dircount.cpp
	$(CXX) $(CXXFLAGS)-o $@ $< 

bin/makeChain: src/makeChain.cpp lib/libRootUtils.so
	$(CXX) $(CXXFLAGS)-o $@ $< -lRootUtils ${AMSSTATICLIBS} ${ROOTLIBS} 

.PHONY: clean
clean:
	rm -f lib/libRootUtils.so lib/libGeneralUtils.so ${OBJS}
