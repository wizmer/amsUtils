#include "GraphFromHistos.hpp"

std::vector<TGraphAsymmErrors*> GraphFromHistos::build(){
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

      std::vector<float> meanValue(nPar);
      std::vector<float> sigmaValue(nPar);
      for(int iPar = 0; iPar < nPar; iPar++){
          meanValue[iPar] = f -> GetParameter( iPar );
          sigmaValue[iPar] = f -> GetParError( iPar );
      }
      
      if( _improveFittingRange ){
          for(int iFit = 0; iFit < numberOfImprovedFits; iFit++){
              histoToFit[ iHisto ] -> Fit( f, option.c_str() , "", meanValue[1] - improvedFitSigmaWidth * f -> GetParameter(2), meanValue[1] + improvedFitSigmaWidth * f -> GetParameter(2) );

              for(int iPar = 0; iPar < nPar; iPar++){
                  meanValue[iPar] = f -> GetParameter( iPar );
                  sigmaValue[iPar] = f -> GetParError( iPar );
              }
          }
      }

      for(int iPar = 0; iPar < nPar; iPar++){
          gr[iPar] -> SetPoint(pointNumber, xCoordinate[ iHisto ], meanValue[iPar]);
          gr[iPar] -> SetPointError(pointNumber,errorXLow[iHisto], errorXHigh[iHisto], sigmaValue[iPar], sigmaValue[iPar]);
      }
      pointNumber++;
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

void GraphFromHistos::init( std::vector<TH1*> _histoToFit, std::vector< float > _xCoordinate, std::string formula) {
  histoToFit = _histoToFit;
  xCoordinate = _xCoordinate;
  _drawFit = false;
  _improveFittingRange = false;
  improvedFitSigmaWidth = true;
  numberOfImprovedFits = 1;
  
  pointNumber = 0;
  sizeHisto = histoToFit.size();
  sizeXCoordinate = xCoordinate.size();

  f = new TF1(formula.c_str(), formula.c_str());
  nPar = f -> GetNpar();

  for(int iPar = 0; iPar < nPar; iPar++) gr.push_back( new TGraphAsymmErrors() );
  
  if( sizeHisto != sizeXCoordinate ){
    std::cout << "size of vec histoToFit (" << sizeHisto << ") is different from size of vec X coordinate (" << sizeXCoordinate << ")" << std::endl;
  }

  errorXLow = std::vector<float>( sizeHisto );
  errorXHigh = std::vector<float>( sizeHisto );
}

