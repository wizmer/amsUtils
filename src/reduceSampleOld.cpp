#include "reduceSampleOld.hpp"

void ReduceSample::init(){
  maxRootFiles = 2000000;
  maxEntries = 1000000;

  Loop::init();
}

bool ReduceSample::process(){
  if (ev==NULL) return 1;
  if (ev->nParticle() != 1) return 2;
  part = ev->pParticle(0);
  tr = (TrTrackR*) part->pTrTrack();
  
  if(!tr) return 3;
  if(ev -> nEcalShower() != 1) return 4;
  shower = part -> pEcalShower();
  if(!shower) return 5;
  EnergyE = shower -> EnergyE;
  std::cout << "EnergyE : " << EnergyE << std::endl;
}

int ReduceSample::cutEvent(){
  return 0;
}


int main( int argc, char** argv ){
  // if( argc < 3 ) {
  //   std::cout << "usage: ./reduceSample inputFile cut" << std::endl;
  // }

  // std::string inputFile = argv[1];
  // std::string cut = argv[2];

  std::string inputFile = "/afs/cern.ch/user/b/bcoste/eos/ams/Data/AMS02/2014/BT.B941/el100/1282147071.00816758.root";
  std::string cut = "ev -> nTrTrack() == 1 && ev -> nEcalShower() == 1";

  TChain* chain = (TChain*)rootUtils::root::get( inputFile, "AMSRoot");
  TTree* newTree = chain -> CopyTree(cut.c_str(), "",1000);
  TFile* file = new TFile("test.root","recreate");
  newTree -> Write();
  file -> Close();

  // chain -> Draw(">>list", cut.c_str(),"entrylist",1000);
  // TEntryList* theList = (TEntryList*) gFile -> Get("list");
  // std::cout << "list : " << theList << std::endl;
  // std::cout << "list -> GetN() : " << theList -> GetN() << std::endl;

  // AMSEventR* ev = 0;
  // chain->SetBranchAddress("ev.",&ev);

  // std::cout << "chain -> GetEntries() : " << chain -> GetEntries() << std::endl;

  // theList -> Print("all");
  // int ientry;
  // while( (ientry = theList -> Next()) > -1 ){
  //   chain -> GetEvent(ientry);
  //   std::cout << ientry << " " << ev->Event() << std::endl; 
  // }
  //  chain -> SetEntryList( theList );
  // std::cout << "chain -> GetEntries() : " << chain -> GetEntries() << std::endl;
  // for (int entry=0;entry < theList->GetN();entry++) {
  //   int entryNumber = chain->GetEntryNumber(entry);
  //   if (entryNumber < 0) break;
  //   int localEntry = chain->LoadTree(entryNumber);
  //   if (localEntry < 0) break;
  //   std::cout << entry << " " << ev->Event() << std::endl;
  //   // then either call branch->GetEntry(localEntry);
  //   // or  entryNumber->GetEntry(entryNumber);
  //   // In the later case the LoadTree is then somewhat redudant.
  // }
  
  exit(1);
  // std::vector < std::string > data;
  // data.push_back( inputFile );
  // ReduceSample t( data, theList );
  // t.go();

}


// void loopChain() {
//   TFile *fe = TFile::Open("myelist.root");
//   TEntryList *myelist = (TEntryList*)fe->Get("myelist");
//   TChain *ch = new TChain("ntuple");
//   ch->Add("hsimple.root");
//   ch->Add("hsimple2.root");
//   Long64_t listEntries = myelist->GetN();
//   Long64_t chainEntries = ch->GetEntries();
//   Int_t treenum = 0;
//   ch->SetEntryList(myelist);
//   for (entry=start;entry < end;entry++) {
//     entryNumber = treechain->GetEntryNumber(entry);
//     if (entryNumber < 0) break;
//     localEntry = fTree->LoadTree(entryNumber);
//     if (localEntry < 0) break;
//     // then either call branch->GetEntry(localEntry);
//     // or  entryNumber->GetEntry(entryNumber);
//     // In the later case the LoadTree is then somewhat redudant.
//   }
// }
