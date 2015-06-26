#ifndef _COMPAREAMSCHAINS__HPP_
#define _COMPAREAMSCHAINS__HPP_

#include <string>
#include <vector>
#include <iostream>

#include "TLine.h"

#include "Loop.h"
#include "rootUtils.hpp"
#include "samples.hpp"
#include "Stack.hpp"

int getEnergy( std::string particuleType );

class CompareAmsChains : public Loop{
 public:
 CompareAmsChains( std::vector< std::string > _data ) : Loop(_data){
    std::cout << "init with #" << data.size() << std::endl;
  }

  void setParticuleType( std::string _particuleType ){
    particuleType = _particuleType;
  }

  void setRunId( std::string _runId ){
    runId = _runId;
  }
    
 private:
  std::string particuleType;
  std::string runId;
  
  float p;
  float R;
  bool isMC;
  int nChi2Cut;

  AMSEventR* newEv;
  TFile* newFile;
  TTree* newChain;  

  TH2F* h2;
  TH2F* h2Full;  

  int newEntry;
  int newEvent;

  float lowEnergyThreshold;
  float highEnergyThreshold;
  TH1F* hDiff;
  
 protected:
  virtual bool process();
  virtual void draw();
  virtual int cutEvent();
  virtual void init();

  bool getEvent(int _event );
};

#endif


