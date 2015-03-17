#ifndef EVENTTOOLS__HPP_
#define EVENTTOOLS__HPP_

namespace EventTools{
  int findMostEnergeticParticle(AMSEventR* ev, int showerId){
    // Looking for the particle having the most energetic shower
    Int_t particleId  =  -1;
    ParticleR* particle;
    for ( UInt_t iParticle  =  0; iParticle < ev -> NParticle(); ++iParticle )
      {
	particle  =  ev -> pParticle( iParticle );
	if ( particle->iEcalShower()  ==  showerId && particle->iTrTrack()  ==  0 )
	  {
	    particleId  =  iParticle;
	    break;
	  }
      }

    return particleId;
  }

  int findMostEnergeticShower(AMSEventR* ev){
    // Find most energetic shower ( if it exists )
    Float_t max_ene   =   0.;
    int showerId = -1;
    int nEcal = ev -> nEcalShower();
    for ( UInt_t iShower  =  0; iShower < nEcal; ++iShower )
      {
	EcalShowerR *shower  =  ev -> pEcalShower( iShower );
	if ( max_ene < shower->EnergyD/1000 )
	  {
	    max_ene   =  shower->EnergyD/1000;
	    showerId  =  iShower;
	  }
      }

    return showerId;
  }
};
  
#endif

