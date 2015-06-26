#ifndef _BENOIT__CPP_
#define _BENOIT__CPP_

#include "rootUtils.hpp"

namespace rootUtils{
    std::string redFontBegin = "\033[1;31m";
    std::string redFontEnd = "\033[0m";

    TObject* get( std::string filename, std::string objectName ){
        TFile* file = (TFile*) gROOT -> GetListOfFiles() -> FindObject( filename.c_str() );
        if( file == NULL ) file = new TFile( filename.c_str() );
        if( file == NULL ){
            std::cout << "No such file !" << std::endl;
            return NULL;
        }

        return file -> Get( objectName.c_str() );
    }

    // return an object contained in the TCanvas
    TObject* getCanvasPrimitive( std::string fileName, std::string canvasName, std::string primitiveName ){
        TCanvas* canvas = (TCanvas*) rootUtils::get(fileName, canvasName);
        if( canvas == NULL ){
            std::cout << rootUtils::redFontBegin << "Canvas : " << canvasName << " not found !" << rootUtils::redFontEnd << std::endl;
            return NULL;
        }

        TObject* obj = canvas -> GetListOfPrimitives() -> FindObject( primitiveName.c_str() );
        if( canvas == NULL ){
            std::cout << rootUtils::redFontBegin << "Primitive : " << primitiveName << " not found !" << rootUtils::redFontEnd << std::endl;
        }
        return obj;
    }

    // Loop on all objects of a file and call the function func on each of them
    void loopOnObjects(TFile* file, void(*func)(TObject*) ){
        TList* list = file -> GetListOfKeys();
        loopOnObjects(list, func);
    }

    // Loop on all objects of a list and call the function func on each of them
    void loopOnObjects(TList* list, void(*func)(TObject*) ){
        // TIter* next = new TIter(list);
        // TKey* key;
        // while ((key = (TKey*)(*next)())){
        // 	TObject* obj = key->ReadObj();
        // 	std::cout << "obj : " << obj << std::endl;
        // 	func(obj);
        // }
        TList* newList = (TList*)list -> Clone();
        int N = newList -> GetEntries();
        for(int i = 0;i<N;i++){
            TObject* obj = newList -> At(i);
            func(obj);
        }
    }

    void setOffset(TH2* h, float offset){
        int nbinx = h -> GetXaxis() -> GetLast() - h -> GetXaxis() -> GetFirst() + 1;
        int nbiny = h -> GetYaxis() -> GetLast() - h -> GetYaxis() -> GetFirst() + 1;
        std::cout << "nbinx : " << nbinx << std::endl;
        std::cout << "nbiny : " << nbiny << std::endl;
  
        double firstX = h -> GetBinLowEdge( h-> GetXaxis() -> GetFirst());
        double lastX = h -> GetBinLowEdge( h -> GetXaxis() -> GetLast() + 1);

        double firstY = h -> GetYaxis() -> GetBinLowEdge( h-> GetYaxis() -> GetFirst());
        double lastY = h -> GetYaxis() -> GetBinLowEdge( h -> GetYaxis() -> GetLast() + 1);

        std::cout << "firstY : " << firstY << std::endl;
        std::cout << "lastY : " << lastY << std::endl;
  
        new TCanvas();
    
        TH2F* hnew = new TH2F("hnew","hnew", nbinx, firstX, lastX, nbiny, firstY + offset, lastY + offset);
        hnew -> SetTitle( h -> GetTitle() );
        std::cout << "hnew -> GetNbinsX() : " << hnew -> GetNbinsX() << std::endl;
        std::cout << "hnew -> GetNbinsY() : " << hnew -> GetNbinsY() << std::endl;
  
        for(int i = 1; i < nbinx; i++){
            for(int j = 1; j < nbiny; j++){
                hnew -> SetBinContent(i,j, h -> GetBinContent(h -> GetXaxis() -> GetFirst() + i, h -> GetYaxis() -> GetFirst() + j));
            }
        }
        hnew -> Draw("colz");
    }
    

    TGraphErrors* averageGraphs( std::vector< TGraphErrors* > vecGr ){
        int nGraphs = vecGr.size();
        if( nGraphs == 0) {
            std::cout << "No graphs in vector !" << std::endl;
            return NULL;
        }

        TGraphErrors* grFinal = new TGraphErrors();
  

        double x,y, errX, errY;
        std::map<float,float> pointArrayY;
        std::map<float,float> pointErrorY;
        std::map<float,int> counter;
  
        for(int i = 0; i<nGraphs; ++i){
            int N = vecGr[i] -> GetN();

            for(int iPoint = 0; iPoint < N; ++iPoint){
                vecGr[i] -> GetPoint(iPoint,x,y);
                errY = vecGr[i] -> GetErrorY(iPoint);

                pointArrayY[x] += y;
                pointErrorY[x] += errY*errY;
                counter[x]++;

            }
        }

        int p = 0;
        for( std::map<float,float>::iterator it = pointArrayY.begin(); it != pointArrayY.end(); ++it){
            y = it -> second / (float)counter[it->first];
            errY = TMath::Sqrt( pointErrorY[it->first])  / (float)counter[it -> first];
            grFinal -> SetPoint(p, it->first,  y);
            std::cout << "pointErrorY[it->first] : " << pointErrorY[it->first] << std::endl;
            std::cout << "errY : " << errY << std::endl;
            grFinal -> SetPointError(p++, 0,  errY);
        }

        return grFinal;
    }

    TGraphErrors* divideGraphs( TGraphErrors* up, TGraphErrors* down  ){
        if( !up || !down ){
            std::cout << "At least one graph pointer is null!" << std::endl;
            std::cout << "Exit !" << std::endl;
            return NULL;
        }

        if( up->GetN() != down->GetN() ){
            std::cout << "Graphs have different sizes !" << std::endl;
            std::cout << "Exit !" << std::endl;
            return NULL;
        }
            
        TGraphErrors* grNew = (TGraphErrors*) up -> Clone();

        double x,y, errX, errY;
        std::map<float,float> pointArrayY;
        std::map<float,float> pointErrorY;
        std::map<float,int> counter;

        int N = up -> GetN();

        for(int iPoint = 0; iPoint < N; ++iPoint){
            down -> GetPoint(iPoint,x,y);
            errY = down -> GetErrorY(iPoint);

            grNew -> SetPoint(iPoint, x,  grNew->GetY()[iPoint]/y);
            grNew -> SetPointError(iPoint, 0,  0);

        }

        return grNew;
    }
    
    TGraphAsymmErrors* scale(TGraphAsymmErrors* gr, float scalingFactor ){
        TGraphAsymmErrors* grNew = (TGraphAsymmErrors*) gr -> Clone();
        int N = gr -> GetN();
        for( int i = 0; i < N; i++){
            double x, y, errYLow, errYHigh;
            grNew -> GetPoint(i,x,y);
            errYLow = grNew -> GetErrorYlow(i);
            errYHigh = grNew -> GetErrorYhigh(i);

            grNew -> SetPoint(i,x,y*scalingFactor);
            grNew -> SetPointEYlow(i, errYLow*fabs(scalingFactor));
            grNew -> SetPointEYhigh(i, errYHigh*fabs(scalingFactor));	
        }
        std::string title = grNew -> GetTitle();
        std::string name = grNew -> GetName();
        grNew -> SetTitle( (title+"_scaled").c_str() );
        grNew -> SetName( (name+"_scaled").c_str() );
        return grNew;
    }

    TGraphErrors* scale(TGraphErrors* gr, float scalingFactor ){
        TGraphErrors* grNew = (TGraphErrors*) gr -> Clone();
        int N = gr -> GetN();
        for( int i = 0; i < N; i++){
            double x, y, errY;
            grNew -> GetPoint(i,x,y);
            errY = grNew -> GetErrorY(i);

            grNew -> SetPoint(i,x,y*scalingFactor);
            grNew -> SetPointError(i, 0,errY*fabs(scalingFactor));
        }
        std::string title = grNew -> GetTitle();
        std::string name = grNew -> GetName();
        grNew -> SetTitle( (title+"_scaled").c_str() );
        grNew -> SetName( (name+"_scaled").c_str() );
        return grNew;
    }
};



#endif
