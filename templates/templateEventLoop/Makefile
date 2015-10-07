# Change the two first lines

EXEC = analyse
ADDCLASS= 

CLASSES = $(ADDCLASS) $(ADDCLASS:include/%.h=src/%.cpp)

BIN    =$(EXEC:%=bin/%)
SRC    =$(EXEC:%=./%.cpp)
OBJECTS=$(EXEC:%=./%.o)


### Options
AMSROOTLIBS   :=$(AMSWD)/include # amschain.h
AMSSTATICLIBS := -L$(AMSWD)/lib/linuxx8664gcc5.34 -lntuple_slc6_PG_dynamic
LIBS	      := -L${amsUtils}/lib -lRootUtils -lGeneralUtils
INCLUDES      := -I. -I$(AMSROOTLIBS) -Iinclude/ -I${amsUtils}/include
DEBUG	      := -g

#Ca va chier dans le compilo
ROOTCFG  := root-config
CXX      :=$(shell $(ROOTCFG) --cxx)   #g++
CXXFLAGS :=$(DEBUG) -O2 $(shell $(ROOTCFG) --cflags) -std=c++11 -D_PGTRACK_ -Wno-write-strings # TH1F...
ROOTLIBS :=$(shell $(ROOTCFG) --libs) -lTMVA -lNetx -lNet -lRFIO


#$?  List of dependencies changed more recently than current target.
#$^  List of dependencies.
#$@  Name of current target.
#$<  Name of current dependency.
#$*  Name of current dependency without extension.


all :$(BIN)

$(BIN) : $(OBJECTS) $(CLASSES)
	$(CXX)  $(CXXFLAGS) $(INCLUDES) -o $@ $^  $(AMSSTATICLIBS) $(ROOTLIBS) $(LIBS) $(amsUtils)/lib/Loop.o


$(OBJECTS): $(SRC)
	$(CXX)  $(CXXFLAGS) $(INCLUDES) -o $@ -c $^

clean:
	rm -rf $(OBJECTS)
