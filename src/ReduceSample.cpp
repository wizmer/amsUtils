#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "TKey.h"
#include "TROOT.h"
#include "TChain.h"
#include "TF1.h"

#include "root.h"

std::string cut;
int nEvent = 1000000000;

void CopyDir(TDirectory *source, TDirectory *savdir, std::string treeName = "AMSRoot" ) {
  static int AMSRootTreeCounter = 0;

  //copy all objects and subdirs of directory source as a subdir of the current directory   
  //  source->ls();
  
  //loop on all entries of this directory
  TKey *key;
  TIter nextkey(source->GetListOfKeys());
  while ((key = (TKey*)nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname);

    if (!cl) continue;
    if (cl->InheritsFrom(TObjString::Class())){
      TObjString* obj = (TObjString*)key -> ReadObj();
      savdir->cd();
      obj -> Write( key -> GetName() );
    }else if (cl->InheritsFrom(TDirectory::Class())) {
      source->cd(key->GetName());
      TDirectory *subdir = gDirectory;
      savdir->cd();
      TDirectory* newDir = savdir->mkdir(key -> GetName());
      newDir -> cd();
      CopyDir(subdir, newDir, treeName);
      savdir->cd();
    } else if (cl->InheritsFrom(TTree::Class())) {
      TTree* obj = (TTree*)key->ReadObj();
      savdir->cd();
      if( strcmp(obj -> GetName(), treeName.c_str() ) == 0 ){
	if( ++AMSRootTreeCounter > 1 ) continue;
	TTree* newTree = obj -> CopyTree(cut.c_str(), "", nEvent);
	newTree -> Write();
      }else{
	std::cout << "normal clone: " << obj -> GetName() << std::endl;
	TTree* AMSRootSetup = obj -> CloneTree();
	AMSRootSetup -> Write();
      }
    } else {
      source->cd();
      TObject *obj = key->ReadObj();
      savdir->cd();
      obj->Write();
    }
  }
  savdir->SaveSelf(kTRUE);
  savdir->cd();
}


int main( int argc, char** argv ){
  std::cout << "argc : " << argc << std::endl;
  if( argc < 3 ) {
    std::cout << "usage: ./ReduceSample inputRootFile TCut [nentries]" << std::endl;
    return 0;
  }


  new TFile("result.root","recreate");
  TDirectory *target = gDirectory;

  std::string fname = argv[1];
  cut = argv[2];
  TObjString theCut(cut.c_str());
  theCut.Write( "selection" );
  if(argc > 3){
    nEvent = atoi(argv[3]);
    std::cout << "nentries to process : " << nEvent << std::endl;
  }
      

  TFile *f = TFile::Open(fname.c_str());
  if (!f || f->IsZombie()) {
    printf("Cannot copy file: %s\n",fname.c_str());
    target->cd();
    return 0;
  }



  CopyDir(f, target, "selections");
  delete f;
  target->cd();

  return 1;
  
}
