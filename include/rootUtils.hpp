#ifndef _ROOTUTILS__HPP_
#define _ROOTUTILS__HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <map>

#include "dirent.h"

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2F.h"
#include "TLegendEntry.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TKey.h"

#include "generalUtils.hpp"

namespace rootUtils{

    TObject* get( std::string filename, std::string objectName );
    
    // return an object contained in the TCanvas
    TObject* getCanvasPrimitive( std::string fileName, std::string canvasName, std::string primitiveName );

    // Loop on all objects of a file and call the function func on each of them
    void loopOnObjects(TFile* file, void(*func)(TObject*) );

    // Loop on all objects of a list and call the function func on each of them
    void loopOnObjects(TList* list, void(*func)(TObject*) );

    
    void setOffset(TH2* h, float offset);
    template< class T> T* setOffset(T* gr, float offset){
        T* grNew = (T*) gr -> Clone();
        int N = gr -> GetN();
        for( int i = 0; i < N; i++){
            double x, y;
            grNew -> GetPoint(i,x,y);
            grNew -> SetPoint(i,x,y+offset);
        }
        return grNew;
    }

    TGraphErrors* averageGraphs( std::vector< TGraphErrors* > vecGr );
    TGraphAsymmErrors* scale(TGraphAsymmErrors* gr, float scalingFactor );
    TGraphErrors* scale(TGraphErrors* gr, float scalingFactor );
    TGraphErrors* divideGraphs( TGraphErrors* up, TGraphErrors* down  );
};


#endif

