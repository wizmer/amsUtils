# Makefile template for shared library

### Options
AMSROOTLIBS   :=$(AMSWD)/include # amschain.h
AMSSTATICLIBS := -L$(AMSWD)/lib/linuxx8664gcc5.34 -Llib -lntuple_slc6_PG
INCLUDES      := -Iinclude -I$(AMSROOTLIBS)
DEBUG	      := -g

#Ca va chier dans le compilo
ROOTCFG  := root-config
CXX      :=$(shell $(ROOTCFG) --cxx)   #g++
CXXFLAGS :=$(INCLUDES) $(DEBUG) -O2 $(shell $(ROOTCFG) --cflags) -D_PGTRACK_ -Wno-write-strings -fPIC # TH1F...
ROOTLIBS :=$(shell $(ROOTCFG) --libs) -lMinuit  -lHistPainter -lMLP -lTree -lTreePlayer  -lXMLIO  -lTMVA  -lRFIO -lNetx

#$?  List of dependencies changed more recently than current target.
#$^  List of dependencies.
#$@  Name of current target.
#$<  Name of current dependency.
#$*  Name of current dependency without extension.


TARGET_LIB = lib/libRootUtils.so # target lib

SRCS = benoit.cpp Loop.cpp Stack.cpp GraphFromHistos.cpp  # source files
OBJS = $(SRCS:.cpp=.o)

all: bin/ReduceSample ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CXX) ${ROOTLIBS} $(INCLUDES) -shared -o $@ $^

$(SRCS:.cpp=.o):%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/ReduceSample: src/ReduceSample.cpp ${TARGET_LIB}
	$(CXX) $(CXXFLAGS)-o $@ $< -lRootUtils ${AMSSTATICLIBS} ${ROOTLIBS} 

.PHONY: clean
clean:
	rm ${TARGET_LIB} ${OBJS}
