#ifndef _ANALYSE__CPP_
#define _ANALYSE__CPP_

#include "analyse.hpp"

using namespace rootUtils;

void Analyse::registerVariables(){
    maxEntries = 1000;

    std::string name = "var1";
    fill[name] = [this,name]() { 
        // compute var1 here:
        float var1 = 0.999 * 2;
        var[name][chunkStepNumber] = var1; 
    };
}

int main(int argc, char **argv){
    TApplication app("app",&argc,argv);

    Analyse t( "/afs/cern.ch/work/b/bcoste/protonB800.root" );
    t.go();

    app.Run();
    return 0;
}

#endif

