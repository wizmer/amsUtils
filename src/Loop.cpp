#ifndef _LOOP__CPP_
#define _LOOP__CPP_

#include "Loop.hpp"

Loop::Loop( std::string _data) : closeOutputFileOnDestroy(true), isSaveAMSTree(false), useList(false), writeList(true), maxEntries(0), maxRootFiles(0), isOutputFile(true){
    data.push_back(_data);
}

Loop::Loop( std::vector< std::string > _data) : data(_data), closeOutputFileOnDestroy(true), isSaveAMSTree(false), useList(false), writeList(true), maxEntries(0), maxRootFiles(0), isOutputFile(true){

}

void Loop::addTree( std::string treePath ){
    data.push_back( treePath );
}

void Loop::loadChain(){
    std::cout << "Loop::loadChain()" << std::endl;
    ch = new AMSChain();
    std::cout << "\033[1;31m";
  
    if(useList){
        int run, event;
        ifstream f(listURL.c_str());
        if(!f.is_open()){
            std::cout << "List file : " << listURL << " not found !" << std::endl;
            exit(-1);
        }
    
        std::map<int, int> mapRun;
        int nFiles = 0;
        while(!f.eof()){
            f >> run >> event;
            if( mapRun[run] == 0 ){
                mapRun[run] = 1;
                nFiles++;
            }
        }
        // int nToLoad = nFiles;
        // int count = 0;

        // if( maxRootFiles > 0 && nFiles > maxRootFiles){
        //   nToLoad = maxRootFiles;
        //   std::cout << "Loading " << nToLoad  << " on a total of " << nFiles  << " root files needed by AMSEventList";
        // }
        // else std::cout << "Loading " << nFiles << " root files needed by AMSEventList";
    
        // for( std::map<int, int>::iterator it = mapRun.begin(); it != mapRun.end();  ++it){
        //   ch -> Add(data[i].c_str());
        //   count++;
        //   if(count >= nToLoad) break;
        // }
    }
  
    {
        int N = data.size();
        if(maxRootFiles > 0 && maxRootFiles < N) std::cout << "Loading " << maxRootFiles << " on a grand total of  # : " << N << std::endl;
        else std::cout << "Loading all root files, # : " << N << std::endl;

        int nTrees = 0;
        
        for(int i = 0;i<N;i++){
            if(maxRootFiles >0 && nTrees >= maxRootFiles) break;

            TFile* file = new TFile(data[i].c_str());
            TObject* obj = file -> Get("AMSRoot");
            if( obj == NULL ) continue;
            // If AMSRoot is a TChain, add the element, one by one
            if( obj -> InheritsFrom("TChain") == true ){
                std::cout << data[i] << " is a TChain, extracting the TTrees..." << std::endl;
                TChain* theChain = (TChain*)obj;
                TObjArray* l = theChain -> GetListOfFiles();
                int nTreesInChain = l -> GetEntries();
                std::cout << nTreesInChain << " TTree found" << std::endl;
                for(int i = 0;i<nTreesInChain;i++){
                    if(maxRootFiles >0 && nTrees >= maxRootFiles) break;
                    TChainElement* el = (TChainElement*)l -> At(i);
                    string aTree(el -> GetTitle());
                    if(nTrees < 5) std::cout << "root file " << nTrees << " : " << aTree.c_str() << std::endl;
                    ch -> Add(aTree.c_str());
                    nTrees++;
                }
            }
            else{
                if(nTrees < 5) std::cout << "root file " << nTrees << " : " << data[nTrees].c_str() << std::endl;
                ch -> Add(data[i].c_str());
                nTrees++;
            }

            file -> Close();
        }
    }
    std::cout << "\033[0m" << std::endl;

}

void Loop::init(){
    std::cout << "Start of Loop::init()" << std::endl;
    firstRun = INT_MAX;
    lastRun = 0;
    outputTree = NULL;
    isOutputTree = true;
    nentriesAfterCuts = 0;
  
    loadChain();

    if( isOutputFile ) openOutputFiles();
  
    nentries = ch->GetEntries();
  
    AMSEventR* ev = 0;
    ch->SetBranchAddress("ev.",&ev);

    setBranchStatuses();
  
    std::cout << "End of Loop::init()" << std::endl;
}

void Loop::loopOnEvents(){
    std::cout << "Loop::loopOnEvents()" << std::endl;

    int theCut;

    if(useList){
        nentries = inputList.GetEntries();
        std::cout << "\033[1;31mUsing an event list.\033[0m" << std::endl;
    }
    std::cout << "nentries : " << nentries << std::endl;
    std::cout << "maxEntries : " << maxEntries << std::endl;
    if(maxEntries > 0 && nentries > maxEntries) nentries = maxEntries;

    std::cout << "\033[1;31m" << "Number of entries to read : " << nentries << "\033[0m" << std::endl;
    bool keepEvent;

    for (entry=0 ;entry < nentries;entry++) {
        if( entry%10000 == 0 || useList ) std::cout << "entry   : " << entry   << std::endl;
        if(useList){
            // ev = ch -> GetEventFast(
            // 			      list.GetRun( static_cast<int>(entry) ),
            // 			      list.GetEvent( static_cast<int>(entry) ),
            // 			      true);
			  
            ev = ch -> GetEvent(
                                inputList.GetRun( static_cast<int>(entry) ),
                                inputList.GetEvent( static_cast<int>(entry) ),
                                true); //  kDontRewind=true the event will be searched starting from current event

        }
        else ev = ch -> GetEvent(entry);

        theCut = cutEvent();
        cut[ theCut ]++;
        if( theCut > 0 ) continue;
        if( process() == false ) continue;
        if( writeList ) addToList();
        if( isSaveAMSTree ) ch -> SaveCurrentEventCont();
        if( outputTree ) {
            outputTree -> Fill();
            // if( nentriesAfterCuts%10000 == 0) outputTree -> AutoSave();
        }
        nentriesAfterCuts++;
    }

    std::cout << "End of Loop::loopOnEvents()" << std::endl;
}

void Loop::fill( std::string nameHisto, float varX, float varY){
    if( h2[nameHisto] == NULL){
        addHisto( nameHisto, 100, 0, 0, 100, 0, 0 );
    }
    h2[nameHisto] -> Fill( varX, varY );
}

void Loop::fill( std::string nameHisto, float var){
    if( h[nameHisto] == NULL){
        addHisto( nameHisto, 100 );
    }
    h[nameHisto] -> Fill( var );
}

void Loop::addHisto( std::string nameHisto, int nBinsX, float firstBinX, float lastBinX, int nBinsY, float firstBinY, float lastBinY ){
    if( h2[nameHisto] != NULL ){
        std::cout << "There is already a histogram called : " << nameHisto << std::endl;
        return;
    }
    h2[nameHisto] = new TH2F( nameHisto.c_str(), nameHisto.c_str(), nBinsX, firstBinX, lastBinX, nBinsY, firstBinY, lastBinY );
}

void Loop::addHisto( std::string nameHisto, int nBins, float firstBin, float lastBin){
    if( h[nameHisto] != NULL ){
        std::cout << "There is already a histogram called : " << nameHisto << std::endl;
        return;
    }
    h[nameHisto] = new TH1F( nameHisto.c_str(), nameHisto.c_str(), nBins, firstBin, lastBin);
}


void Loop::setEventList( std::string _listName ){
    listURL = _listName;
    useList = true;
    writeList = false;
    inputList.Read( listURL.c_str() );
}

void Loop::addToList(){
    outputList.Add(ev);
    if(ev -> Run() < firstRun) firstRun = ev -> Run();
    if(ev -> Run() > lastRun) lastRun = ev -> Run();
}

void Loop::openOutputFiles(){
    generalUtils::makeFolder("output");
    generalUtils::makeFolder("lists");

    TFile* currentFile = gFile;
  
    if( outputFileName == "" ){
        outputFileName = "output/output_";
        outputFileName += generalUtils::dateTime();
        outputFileName += ".root";
    }

    if( isSaveAMSTree ){
        ch -> OpenOutputFile( outputFileName.c_str() );
        outputFile = (TFile*) gROOT -> GetListOfFiles() -> FindObject( outputFileName.c_str() );
    }
    else outputFile = new TFile(outputFileName.c_str(),"recreate");

    outputTree = new TTree("tree","tree");
    setOutputBranches();
  
    registerSrcFilesInRootuple();

    if( currentFile ) currentFile -> cd();
}

void Loop::registerSrcFilesInRootuple(){
    std::vector <std::string > files = generalUtils::getFilesInDir(".");
    TDirectory *srcDir = outputFile -> mkdir("src");
    srcDir -> cd();

    for(int i = 0; i < files.size(); i++){
        string extension = generalUtils::getExtension( files[i] );
        if( extension == "cpp" || extension == "hpp" || extension == "h" || extension == "c" || extension == "C" || extension == "cxx" || extension == "hxx"){
            TMacro m( generalUtils::getFileName(files[i]).c_str() );
            m.Write( generalUtils::getFileName(files[i]).c_str() );
        }
    }
  
    outputFile -> cd();
    
}

void Loop::writePrivate(){
    if( isOutputFile ){
        outputFile -> cd();
        if( writeList ) {
            if( listName == "" ) listName = Form("lists/list_%i_%i.txt",firstRun, lastRun);
            outputList.Write( listName.c_str() );
        }

        for( std::map< std::string, TH1* >::iterator it = h.begin(); it != h.end();  ++it){
            if(it -> second != NULL){
                it -> second -> Write();
            }
        }

        for( std::map< std::string, TH2* >::iterator it = h2.begin(); it != h2.end();  ++it){
            if(it -> second != NULL){
                it -> second -> Write();
            }
        }

        for( std::map< std::string, TGraph* >::iterator it = gr.begin(); it != gr.end();  ++it){
            if(it -> second != NULL){
                it -> second -> Write(it -> first.c_str());
            }
        }

        if( outputTree ) outputTree -> Write();

        // User function write
        write();
    
        if( isSaveAMSTree ) ch -> CloseOutputFile();
        else outputFile -> Write();
    }
}

void Loop::dumpCuts(){
    int nCut = 0;

    for( std::map<int,int>::iterator it = cut.begin(); it != cut.end(); ++it){
        if( it -> first > nCut ) nCut = it -> first;
    }

    std::cout << "Cut number - #entries" << std::endl;
    for(int iCut = 0; iCut <= nCut; iCut++){
        std::cout << iCut << " - " << cut[iCut] << std::endl;
    }
}

#endif
