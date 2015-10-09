#ifndef _ANALYSE__HPP_
#define _ANALYSE__HPP_

#include <string>
#include <vector>
#include <iostream>

#include "DstAmsBinary.hpp"
#include "rootUtils.hpp"
#include "Stack.hpp"

class Analyse : public DstAmsBinary{
public:
    Analyse( std::string _data ) : DstAmsBinary( _data){
        std::cout << "init with #" << data.size() << std::endl;
    }
  
protected:
    // virtual void init();
    void registerVariables();
};

#endif


