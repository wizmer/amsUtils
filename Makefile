# Makefile template for shared library

### Options
AMSROOTLIBS   :=$(AMSWD)/include # amschain.h
AMSSTATICLIBS := -L$(AMSWD)/lib/linuxx8664gcc5.34 -Llib -lntuple_slc6_PG
INCLUDES      := -Iinclude
ROOT_INCLUDES := -I$(AMSROOTLIBS)
DEBUG	      := -g

#Ca va chier dans le compilo
ROOTCFG  := root-config
#CXX      :=$(shell $(ROOTCFG) --cxx)   #g++
CXX	:= gcc
CXXFLAGS :=$(INCLUDES) $(ROOT_INCLUDES) $(DEBUG) -O2 $(shell $(ROOTCFG) --cflags) -D_PGTRACK_ -Wno-write-strings -fPIC # TH1F...
ROOTLIBS :=$(shell $(ROOTCFG) --libs) -lMinuit  -lHistPainter -lMLP -lTree -lTreePlayer  -lXMLIO  -lTMVA  -lRFIO -lNetx

#$?  List of dependencies changed more recently than current target.
#$^  List of dependencies.
#$@  Name of current target.
#$<  Name of current dependency.
#$*  Name of current dependency without extension.


SRCS = generalUtils.cpp rootUtils.cpp Loop.cpp Stack.cpp GraphFromHistos.cpp  # source files
OBJS = $(SRCS:.cpp=.o)

all: lib/libGeneralUtils.so bin/ReduceSample bin/makeChain lib/libRootUtils.so

lib/libGeneralUtils.so: generalUtils.o
	$(CXX) $(INCLUDES) -shared -o $@ $^

lib/libRootUtils.so: $(OBJS)
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^

$(SRCS:.cpp=.o):%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/ReduceSample: src/ReduceSample.cpp lib/libRootUtils.so
	$(CXX) $(CXXFLAGS)-o $@ $< -lRootUtils ${AMSSTATICLIBS} ${ROOTLIBS} 

bin/makeChain: src/makeChain.cpp lib/libRootUtils.so
	$(CXX) $(CXXFLAGS)-o $@ $< -lRootUtils ${AMSSTATICLIBS} ${ROOTLIBS} 

.PHONY: clean
clean:
	rm -f lib/libRootUtils.so lib/libGeneralUtils.so ${OBJS}
