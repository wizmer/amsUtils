#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "TKey.h"
#include "TROOT.h"
#include "TChain.h"
#include "TF1.h"

#include "rootUtils.hpp"
#include "Stack.hpp"

int main(int argc, char** argv){
  if(argc < 2) {
    std::cout << "usage : root macro.cpp fileDir outFileName" << std::endl;
    return 0;
  }

  std::string fileDir(argv[1]);  
  std::string outFileName(argv[2]);
  
  std::vector <std::string > files = generalUtils::getFilesInDirWithPattern( fileDir, "root");
  TFile* file = new TFile(outFileName.c_str() ,"recreate");
  TChain* chain = new TChain();

  std::cout << "files.size() : " << files.size() << std::endl;
  for(int i = 0; i< files.size();++i){
    std::cout << files[i] + "/AMSRoot" << std::endl;
    chain -> Add( (files[i] + "/AMSRoot").c_str() );
  }

  chain -> Write("AMSRoot");
  file -> Close();

  return 1;
}
