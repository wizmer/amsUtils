#include <string>
#include <vector>
#include <iostream>

#include "Loop.hpp"
#include "rootUtils.hpp"

#include "Stack.hpp"
#include "TEntryList.h"

class ReduceSample : public Loop{
public:

  ReduceSample( std::vector< std::string > _data, TEntryList* _list ) : Loop(_data), list(_list) {
    std::cout << "init with #" << data.size() << std::endl;
  }

private:
  TEntryList* list;
  
protected:
  virtual bool process();
  virtual int cutEvent();
  virtual void init();
};

