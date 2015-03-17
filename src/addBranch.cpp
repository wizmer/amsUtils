#efndef _ADDBRANCH__CPP_
#define _ADDBRANCH__CPP_

void tree3AddBranch() {
  TFile f("tree3.root","update");
  Float_t new_v;
  TTree *t3 = (TTree*)f->Get("t3");
  TBranch *newBranch = t3-> Branch("new_v",&new_v,"new_v/F");
  //read the number of entries in the t3
  Int_t nentries = (Int_t)t3->GetEntries();
  for (Int_t i = 0; i < nentries; i++){
    new_v= gRandom->Gaus(0,1);
    newBranch->Fill();
  }
  t3->Write("",TObject::kOverwrite);     // save only the new version of the tree
}

#endif
