#ifndef _BENOIT__HPP_
#define _BENOIT__HPP_

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
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TKey.h"

namespace benoit{

  namespace root{
    TObject* get( std::string filename, std::string objectName );
    
    // Loop on all objects of a file and call the function func on each of them
    void loopOnObjects(TFile* file, void(*func)(TObject*) );

    // Loop on all objects of a list and call the function func on each of them
    void loopOnObjects(TList* list, void(*func)(TObject*) );

    
    void setOffset(TH2* h, float offset);
    void setOffset(TGraph* gr, float offset);
    TGraphErrors* averageGraphs( std::vector< TGraphErrors* > vecGr );
  };
  
  std::map< std::string, float > getEnergyBeamTest();
  std::map< std::string, std::string > getEnergyBeamTestString();
  int getColor(int n, bool fill = 0);

  std::string getPath(std::string fullFileName);
  std::string getFileName(std::string fullFileName);
  std::string getExtension( std::string fileName );

  std::vector <std::string > getFilesInDir(std::string dirName, int maxNumberOfFile = 0);
  std::vector <std::string > getFilesInDirWithPattern(std::string dirName, std::string pattern);

  std::string dateTime();
  bool folderExists( std::string folderName );
  int makeFolder( std::string folderName );

  // return STDOUT from the shell command: cmd (but not STDERR)
  std::string exec(std::string cmd);

  // split a string containing line returns into several strings
  std::vector<std::string> splitIntoLines(const std::string &string);
  std::vector< std::string > split( std::string str, std::string delimiters );
  // Same as Split but with reverse ordering
  std::vector< std::string > splitFromEnd( std::string str, std::string delimiters );
  std::string replacePattern(std::string str, const std::string& oldPattern, const std::string& newPattern);

  template<class T> T stringTo(std::string str){
    std::stringstream ss;
    T res;
    ss << str;
    ss >> res;
    return res;
  }

  std::string toString(float a);
};

#endif

