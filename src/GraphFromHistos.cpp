#include "GraphFromHistos.hpp"

TGraphAsymmErrors* GraphFromHistos::build(){
  std::string option;
  if ( _drawFit == false ) option = "N"; // do not store graphical function
  
  for( int iHisto = 0; iHisto < sizeHisto; ++iHisto ){
    if( f != NULL && histoToFit[ iHisto ] && histoToFit[ iHisto ] -> GetEntries() > 0 ){
      if( _drawFit == true ) new TCanvas();
      if( histoToFit[ iHisto ] == NULL ){
          std::cout << "Null pointer for : histoToFit["<<iHisto<<"]" << std::endl;
          continue;
      }
      if( _drawFit == true ) histoToFit[ iHisto ] ->Draw();
      histoToFit[ iHisto ] -> Fit( f, option.c_str() , "same", borneInfFit[iHisto], borneSupFit[iHisto] );
      float meanValue = f -> GetParameter( parameter );
      float sigmaValue = f -> GetParError( parameter );
      if( _improveFittingRange ){
          for(int iFit = 0; iFit < numberOfImprovedFits; iFit++){
              histoToFit[ iHisto ] -> Fit( f, option.c_str() , "", meanValue - improvedFitSigmaWidth * f -> GetParameter(2), meanValue + improvedFitSigmaWidth * f -> GetParameter(2) );
              meanValue = f -> GetParameter( parameter );
              sigmaValue = f -> GetParError( parameter );
          }
      }
      gr -> SetPoint(pointNumber, xCoordinate[ iHisto ], meanValue);
      gr -> SetPointError(pointNumber++,errorXLow[iHisto], errorXHigh[iHisto], sigmaValue, sigmaValue);
    }
  }
  return gr;
}

void GraphFromHistos::setFittingRange(float _borneInfFit, float _borneSupFit){
  int N = histoToFit.size();
  borneInfFit.clear();
  borneSupFit.clear();
  for(int i = 0;i<N;i++){
    borneInfFit.push_back(_borneInfFit);
    borneSupFit.push_back(_borneSupFit);
  }
}

void GraphFromHistos::setFittingRange(std::vector<float> _borneInfFit, std::vector<float> _borneSupFit){
  borneInfFit = _borneInfFit;
  borneSupFit = _borneSupFit;
}

void GraphFromHistos::setXErrors( std::vector<float> _errorXLow, std::vector<float> _errorXHigh ){
  errorXLow = _errorXLow;
  if( _errorXHigh.size() > 0 ){
    errorXHigh = _errorXHigh;
  }else{
    errorXHigh = errorXLow;
  }
}

void GraphFromHistos::init( std::vector<TH1*> _histoToFit, std::vector< float > _xCoordinate, std::string formula, int _parameter ) {
  histoToFit = _histoToFit;
  xCoordinate = _xCoordinate;
  parameter = _parameter;
  _drawFit = false;
  _improveFittingRange = false;
  improvedFitSigmaWidth = true;
  numberOfImprovedFits = 1;
  
  gr = new TGraphAsymmErrors();

  pointNumber = 0;
  sizeHisto = histoToFit.size();
  sizeXCoordinate = xCoordinate.size();

  f = new TF1(formula.c_str(), formula.c_str());

  if( sizeHisto != sizeXCoordinate ){
    std::cout << "size of vec histoToFit (" << sizeHisto << ") is different from size of vec X coordinate (" << sizeXCoordinate << ")" << std::endl;
  }

  errorXLow = std::vector<float>( sizeHisto );
  errorXHigh = std::vector<float>( sizeHisto );
}

