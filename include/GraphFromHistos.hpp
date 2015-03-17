#ifndef _GRAPHFROMHISTOS__HPP_
#define _GRAPHFROMHISTOS__HPP_

#include <vector>
#include <string>
#include <map>

#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TF1.h"

class GraphFromHistos{
public:
  GraphFromHistos( std::vector<TH1*> _histoToFit, std::vector< float > _xCoordinate, std::string formula, int _parameter ){
    init( _histoToFit, _xCoordinate, formula, _parameter );
  }

  GraphFromHistos( std::map<std::string, TH1*> _histoToFit, std::map<std::string, float > _xCoordinate, std::string _formula, int _parameter ){
    std::vector<TH1*> vecHisto;
    std::vector<float > vecXCoordinate;
    for( std::map<std::string, TH1*>::iterator it = _histoToFit.begin(); it != _histoToFit.end(); ++it ) vecHisto.push_back( it->second );
    for( std::map<std::string, float>::iterator it = _xCoordinate.begin(); it != _xCoordinate.end(); ++it ) vecXCoordinate.push_back( it->second );
    init( vecHisto, vecXCoordinate, _formula, _parameter );
  }
      
  TGraphAsymmErrors* build();
      
  void setFittingRange(float _borneInfFit, float _borneSupFit);
  void setFittingRange(std::vector<float> _borneInfFit, std::vector<float> _borneSupFit);
  void setXErrors( std::vector<float> _errorXLow, std::vector<float> _errorXHigh = std::vector<float>());

protected:
  void init( std::vector<TH1*> _histoToFit, std::vector< float > _xCoordinate, std::string formula, int _parameter );

private:
  std::vector<TH1*> histoToFit;
  std::vector<float> xCoordinate;
  std::vector<float> borneInfFit, borneSupFit;
  std::vector<float> errorXLow, errorXHigh;

  TGraphAsymmErrors* gr;
  TF1* f;
  float sizeHisto;
  float sizeXCoordinate;
  float pointNumber;
  int parameter; // The TF1 parameter to use for making the TGraph::SetPoint
};

#endif

