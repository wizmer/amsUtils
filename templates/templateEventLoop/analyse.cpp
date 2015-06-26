#ifndef _ANALYSE__CPP_
#define _ANALYSE__CPP_

#include "analyse.hpp"

using namespace rootUtils;

void Analyse::init(){
  maxRootFiles = 2;
  maxEntries = 1000;

  Loop::init();
}

bool Analyse::process(){

}


void Analyse::draw(){
}

int Analyse::cutEvent(){
  if (ev==NULL) return 1;
  if (ev->nParticle() != 1) return 2;
  part = ev->pParticle(0);
  tr = (TrTrackR*) part->pTrTrack();
  
  if(!tr) return 3;
  if(ev -> nEcalShower() != 1) return 4;

  return 0;
}

int main(int argc, char **argv){
  TApplication app("app",&argc,argv);

  std::vector< std::string > data;

  Analyse t( data );
  //  t.setEventList("list.txt");
  t.go();

  app.Run();
  return 0;
}

#endif

