#ifndef _SAMPLES__HPP_
#define _SAMPLES__HPP_

#include <vector>
#include <string>
#include <iostream>

#include "rootUtils.hpp"

namespace DataPathFinder{

  enum DataSet {
    kProtonMCB620,
    kPositronMC,
    kElectronMCB620,
    kElectronbt100gev,
    kData
  };

  bool ElectronMCB620(std::vector< std::string > &data){
    data =  rootUtils::getFilesInDir("/storage/gpfs_ams/ams/Rec/MC/2011B/el.B620dev/el.pl1.0_255");
    bool isMC = true;
    return isMC;
  }

  bool ProtonMCB620(std::vector< std::string > &data){
    data =  rootUtils::getFilesInDir("/storage/gpfs_ams/ams/Rec/MC/2011B/protons.B620dev/pr.pl1.ecal.256");
    bool isMC = true;
    return isMC;
  }

  bool PositronMC(std::vector< std::string > &data){
    data =  rootUtils::getFilesInDir("/storage/gpfs_ams/ams/Rec/MC/2011B/pos.B598.rd.B620dev/pos.pl1.5100");
    bool isMC = true;
    return isMC;
  }

  bool Electronbt100gev(std::vector< std::string > &data){
    data =  rootUtils::getFilesInDir("/storage/gpfs_ams/ams/Rec/2014/BT.B700/el100/");
    bool isMC = true;
    return isMC;
  }
  bool Data(std::vector< std::string > &data){
    data = rootUtils::getFilesInDir("/storage/gpfs_ams/ams/Rec/2014/ISS.B700/pass5/");
    bool isMC = false;
    return isMC;
  }

  bool getDataSet(DataSet aDataSet, std::vector< std::string > &data ){
    switch( aDataSet ){
	case kProtonMCB620 :
	  return ProtonMCB620(data);
	  
	case kPositronMC :
	  return PositronMC(data);
	  
	case kElectronMCB620 :
	  return ElectronMCB620(data);
	  
	case kElectronbt100gev :
	  return Electronbt100gev(data);
	  
	case kData :
	  return Data(data);
    }
    return false;

  }
}
#endif
