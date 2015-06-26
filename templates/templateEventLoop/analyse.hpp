#ifndef _ANALYSE__HPP_
#define _ANALYSE__HPP_

#include <string>
#include <vector>
#include <iostream>

#include "Loop.hpp"
#include "rootUtils.hpp"
#include "Stack.hpp"

class Analyse : public Loop{
public:
  Analyse( std::vector< std::string > _data ) : Loop(_data){
    std::cout << "init with #" << data.size() << std::endl;
  }
  
  Analyse( std::string _data ) : Loop(_data){
    std::cout << "init with #" << data.size() << std::endl;
  }
  
private:
  float p;
  float R;
  bool isMC;
  int nChi2Cut;
  
protected:
  virtual bool process();
  virtual void draw();
  virtual int cutEvent();
  virtual void init();
};

#endif


