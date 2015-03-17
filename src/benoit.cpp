#ifndef _BENOIT__CPP_
#define _BENOIT__CPP_

#include "benoit.hpp"

namespace benoit{
  namespace root{
    TObject* get( std::string filename, std::string objectName ){
      TFile* file = (TFile*) gROOT -> GetListOfFiles() -> FindObject( filename.c_str() );
      if( file == NULL ) file = new TFile( filename.c_str() );
      if( file == NULL ){
	std::cout << "No such file !" << std::endl;
	return NULL;
      }

      return file -> Get( objectName.c_str() );
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
    
    void setOffset(TGraph* gr, float offset){
      TGraph* grNew = (TGraph*) gr -> Clone();
      int N = gr -> GetN();
      for( int i = 0; i < N; i++){
	double x, y;
	grNew -> GetPoint(i,x,y);
	grNew -> SetPoint(i,x,y+offset);
      }
      grNew -> Draw("p");
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
  }


  std::map< std::string, float > getEnergyBeamTest(){
    std::map< std::string, float > energyBeamTest;
    energyBeamTest["pos10"]  = 10;
    energyBeamTest["pos20"]  = 20;
    energyBeamTest["pos80"]  = 80;
    energyBeamTest["pos100"] = 100;
    energyBeamTest["pos120"] = 119.9;    
    energyBeamTest["pos180"] = 179.5;

    energyBeamTest["el100"] = 99.9;
    energyBeamTest["el120"] = 119.9;
    energyBeamTest["el180"] = 178.6;
    energyBeamTest["el300"] = 290.8;

    return energyBeamTest;
  }

  std::map< std::string, std::string > getEnergyBeamTestString(){
    std::map< std::string, std::string > energyBeamTestString;
    std::map< std::string, float > energyBeamTest = benoit::getEnergyBeamTest();

    for( std::map< std::string, float>::iterator it = energyBeamTest.begin(); it != energyBeamTest.end(); ++it){
      std::stringstream ss;
      std::string energyString;
      ss << it -> second;
      ss >> energyString;
      energyBeamTestString[ it -> first ] = energyString;
    }

    return energyBeamTestString;
  }
  
  int getColor(int n, bool fill ){
    int col[8]={4, 2, 8, 1, 7, 46, 6, 49};
    int color=n<8?col[n]:5+n;
    if (n==0 && fill) color=38;
    return color;
  }

  std::string getPath(std::string fullFileName){
    //Get old file, old tree and set top branch address
    std::string path;
    std::string fileName;

    size_t pos = fullFileName.find_last_of('/');
    if(pos != std::string::npos){
      path = fullFileName.substr(0,pos+1);
    }
    return path;
  }  

  std::string getFileName(std::string fullFileName){
    //Get old file, old tree and set top branch address
    std::string path;
    std::string fileName;

    size_t pos = fullFileName.find_last_of('/');
    if(pos == std::string::npos){
      fileName = fullFileName;
    }else{
      fileName = fullFileName.substr(pos+1, fullFileName.length());
    }
    return fileName;
  }

  std::string getExtension( std::string fileName ){
    std::string extension;
    
    size_t pos = fileName.find_last_of('.');
    if(pos != std::string::npos){
      extension = fileName.substr(pos+1, fileName.length());
    }
    return extension;
  }

  std::vector <std::string > getFilesInDir(std::string dirName, int maxNumberOfFile ){
    std::vector < std::string > files;
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    if ((dir = opendir (dirName.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
	if(maxNumberOfFile > 0 && i >= maxNumberOfFile) break;
	std::string file(ent->d_name);
	if(file == "." || file == "..") continue;
	files.push_back( dirName+'/'+file );
	i++;
      }
      closedir (dir);
    }
    return files;
  }

  std::vector <std::string > getFilesInDirWithPattern(std::string dirName, std::string pattern){
    std::vector < std::string > files = getFilesInDir( dirName );
    std::vector < std::string > filesWithPattern;
    for(int i = 0; i < files.size(); ++i){
      size_t found = files[i].find( pattern );
      if( found != std::string::npos ) filesWithPattern.push_back( files[i] );
    }
    return filesWithPattern;
  }

  std::string dateTime(){
    time_t now = time(0);
    tm *ltm = localtime(&now);

    std::stringstream ss;
    ss << 1900 + ltm->tm_year;
    if(1 + ltm->tm_mon < 10) ss << 0;
    ss << 1 + ltm->tm_mon;
    if( ltm->tm_mday < 10) ss << 0;
    ss << ltm->tm_mday << '_';
    if( ltm->tm_hour < 10) ss << 0;
    ss << ltm->tm_hour;
    if( ltm->tm_min < 10) ss << 0;
    ss <<  ltm->tm_min;
    if( ltm->tm_sec < 10) ss << 0;
    ss << ltm->tm_sec;
    return ss.str();
  }

  bool folderExists( std::string folderName ){
    struct stat st;
    stat(folderName.c_str() , &st);
    return S_ISDIR(st.st_mode);
  }

  int makeFolder( std::string folderName ){
    return mkdir( folderName.c_str() , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }

  // return STDOUT from the shell command: cmd (but not STDERR)
  std::string exec(std::string cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL)
	result += buffer;
    }
    pclose(pipe);
    return result;
  }

  std::vector<std::string> splitIntoLines(const std::string &string){
    std::stringstream stream(string);
    std::vector<std::string> res;
    while (1){
      std::string line;
      std::getline(stream,line);
      if (!stream.good())
	break;
      res.push_back(line);
    }
    return res;
  }

  std::vector< std::string > split( std::string str, std::string delimiters ){
    std::vector < std::string > res;
    if( str.length() == 0 ) return res;

    std::size_t pos;

    do{
      pos = str.find_first_of( delimiters );
      //    cout << "avant : " << str << endl; 
      std::string a = str.substr(0,pos);
      if( a.length() > 0) res.push_back(a);
      str = str.substr(pos+1);
      //    cout << "apres : " << str << endl;
    } while( pos != std::string::npos );

    return res;
  }

  // Split in reverse order
  std::vector< std::string > splitFromEnd( std::string str, std::string delimiters ){
    std::vector< std::string > splittedString = benoit::split( str, delimiters );
    std::reverse( splittedString.begin(), splittedString.end() );
    return splittedString;
  }

  std::string replacePattern(std::string str, const std::string& oldPattern, const std::string& newPattern)
  {
    std::string result = str;
    size_t pos = 0;
    while((pos = result.find(oldPattern, pos)) != std::string::npos)
      {
	result.replace(pos, oldPattern.length(), newPattern);
	pos += newPattern.length();
      }

    return result;
  }

  std::string toString(float a){
    std::stringstream ss;
    std::string res;
    ss << a;
    ss >> res;
    return res;
  }
}

#endif
