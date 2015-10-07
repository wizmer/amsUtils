#ifndef _GRAPHFROMHISTOS__HPP_
#define _GRAPHFROMHISTOS__HPP_

#include <vector>
#include <string>
#include <map>

#include "TGraphAsymmErrors.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TF1.h"

// This class is used to automatize the fitting procedure in the case you have multiple histogram to fit with the same function
// It will fit every histogram by the user function and put the value of the function parameter of interest in a TGraphAsymmErrors
// input: a vector of TH1, a vector of float representing the TGraphAsymmErrors X coordinate that has to be associated with every histogram
// input: the formula of the function you want to fit
// input: the function parameter that you want to extract
class GraphFromHistos{
public:
    GraphFromHistos( std::vector<TH1*> _histoToFit, std::vector< float > _xCoordinate, std::string formula){
        init( _histoToFit, _xCoordinate, formula);
    }

    GraphFromHistos( std::map<std::string, TH1*> _histoToFit, std::map<std::string, float > _xCoordinate, std::string _formula ){
        std::vector<TH1*> vecHisto;
        std::vector<float > vecXCoordinate;
        for( std::map<std::string, TH1*>::iterator it = _histoToFit.begin(); it != _histoToFit.end(); ++it ) vecHisto.push_back( it->second );
        for( std::map<std::string, float>::iterator it = _xCoordinate.begin(); it != _xCoordinate.end(); ++it ) vecXCoordinate.push_back( it->second );
        init( vecHisto, vecXCoordinate, _formula );
    }

    // Use a TH2 to generate the list of 1D histogram to be fitted.
    // Here the 1D histograms are obtained by Y-projecting the 2D histo for every X bin
    GraphFromHistos( TH2* h2, std::string _formula, int nBinsPerPoint = 1){
        int N = h2 -> GetNbinsX() / nBinsPerPoint;

        std::vector< TH1* > h;
        std::vector< float > x;
        for(int i = 0;i < N;i++){
            TH1D* hproj = h2 -> ProjectionY( Form("hproj_%i", i), i*nBinsPerPoint + 1, (i+1)*nBinsPerPoint );
            h.push_back( hproj );

            float borneInf = h2 -> GetXaxis() -> GetBinLowEdge( i*nBinsPerPoint );
            float borneSup = h2 -> GetXaxis() -> GetBinLowEdge( (i+1)*nBinsPerPoint + 1);
            x.push_back( 0.5*(borneInf+borneSup) );
        }
        init( h, x, _formula );
    }

    ~GraphFromHistos(){
        histoToFit.clear();
        xCoordinate.clear();
    }
  
    std::vector<TGraphAsymmErrors*> build();
      
    void setFittingRange(float _borneInfFit, float _borneSupFit);
    void setFittingRange(std::vector<float> _borneInfFit, std::vector<float> _borneSupFit);
    void setXErrors( std::vector<float> _errorXLow, std::vector<float> _errorXHigh = std::vector<float>());
    void drawFit( bool __drawFit ){
        _drawFit = __drawFit;
    }

    void improveFittingRange( bool __improveFittingRange ){
        _improveFittingRange = __improveFittingRange;
    }

    void setImprovedFitSigmaWidth( float _improvedFitSigmaWidth ){
        improvedFitSigmaWidth = _improvedFitSigmaWidth;
    }

    void setNumberOfImprovedFits( int N ){
        numberOfImprovedFits = N;
    }

protected:
    void init( std::vector<TH1*> _histoToFit, std::vector< float > _xCoordinate, std::string formula );

private:
    std::vector<TH1*> histoToFit;
    std::vector<float> xCoordinate;
    std::vector<float> borneInfFit, borneSupFit;
    std::vector<float> errorXLow, errorXHigh;

    std::vector<TGraphAsymmErrors*> gr;
    TF1* f;
    float sizeHisto;
    float sizeXCoordinate;
    float pointNumber;
    bool _drawFit;
    bool _improveFittingRange;
    float improvedFitSigmaWidth;
    int numberOfImprovedFits;
    int nPar;
};

#endif

