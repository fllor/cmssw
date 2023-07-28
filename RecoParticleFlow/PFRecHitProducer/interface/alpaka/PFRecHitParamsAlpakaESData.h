#ifndef RecoParticleFlow_PFRecHitProducer_alpaka_PFRecHitParamsAlpakaESData_h
#define RecoParticleFlow_PFRecHitProducer_alpaka_PFRecHitParamsAlpakaESData_h

#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitParamsAlpakaESData.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitParamsAlpakaESDataSoA.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using PFRecHitHCALParamsAlpakaESDataHost = reco::PFRecHitHCALParamsAlpakaESDataHost;
  using PFRecHitHCALParamsAlpakaESDataDevice = PortableCollection<reco::PFRecHitHCALParamsAlpakaESDataSoA>;

  using PFRecHitECALParamsAlpakaESDataHost = reco::PFRecHitECALParamsAlpakaESDataHost;
  using PFRecHitECALParamsAlpakaESDataDevice = PortableCollection<reco::PFRecHitECALParamsAlpakaESDataSoA>;

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif
