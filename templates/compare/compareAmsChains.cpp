#ifndef _COMPAREAMSCHAINS__CPP_
#define _COMPAREAMSCHAINS__CPP_

#include "compareAmsChains.hpp"

using namespace benoit;

void CompareAmsChains::init(){
  maxRootFiles = 2;
  //  maxEntries = 1000;

  Loop::init();
  
  std::string theData = "/afs/cern.ch/user/b/bcoste/myeos/finalMipValue/el180/el180_1282179036_Bdevv.root";
  newChain = (TTree*) rootUtils::root:Get(theData, "AMSRoot");

  newEntry = -1;
  newEvent = -1;

  newEv = 0;
  newChain -> SetBranchAddress("ev.", &newEv);
}



bool CompareAmsChains::process(){
  unsigned int run = ev -> Run();
  unsigned int event = ev -> Event();

  if(!getEvent(event)){
    std::cout << "event : " << event << " not found" << std::endl;
    std::cout << "Exit !" << std::endl;
    exit(-1);
  }

  if(  newEv -> nEcalShower() > 0 && newEv -> pEcalShower(0) != 0 ){
    float oldEnergyE = ev -> pEcalShower(0) -> EnergyE;
    float newEnergyE = newEv -> pEcalShower(0) -> EnergyE;
    // h2 -> Fill(oldEnergyE, newEnergyE );
    // h2Full -> Fill(oldEnergyE, newEnergyE );    
    // hDiff -> Fill( (newEnergyE - oldEnergyE) / (newEnergyE + oldEnergyE) / 2 * 100.);
  }
}



int CompareAmsChains::cutEvent(){
  int retVal = 0;
  if (ev == NULL) retVal = 1;
  else if(ev -> nEcalShower() < 1) retVal = 2;
  else if( ev -> pEcalShower(0) == 0 ) retVal = 3;
  else if( ev -> pEcalShower(0) -> EnergyE < lowEnergyThreshold || ev -> pEcalShower(0) -> EnergyE > highEnergyThreshold ) retVal = 4;
  return retVal;
}

bool CompareAmsChains::getEvent(int _event ){
  while( newEvent < _event){
    newChain -> GetEntry(++newEntry);
    newEvent = newEv -> Event();
  }

  if(newEvent != _event) return false;
  return true;
}

int main(int argc, char **argv){
  TApplication app("app",&argc,argv);

  if (argc < 3){
    std::cout << "usage : bin/compareAmsChains particuleFolder runNumber" << std::endl;
    return 0;
  }

  std::string particuleType(argv[1]);
  std::string runId(argv[2]);

  std::vector< std::string > data;
  std::string theData = "$HOME/eos/ams/Data/AMS02/2014/BT.B700/" + particuleType + "/" + runId + ".00000001.root";
  data.push_back(theData);

  CompareAmsChains t( data );
  t.setRunId( argv[2] );
  t.setParticuleType( argv[1] );
  //  t.setEventList("list.txt");
  t.run();

  //   app.Run();
  return 1;
}

int getEnergy( std::string particuleType ){
  std::size_t pos = particuleType.find_first_of("0123456789");
  std::string energyStr = particuleType.substr(pos);
  std::stringstream ss;
  ss << energyStr;
  int res;
  ss >> res;
  return res;
}

#endif

