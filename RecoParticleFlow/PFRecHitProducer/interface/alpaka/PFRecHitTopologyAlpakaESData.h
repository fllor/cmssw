#ifndef RecoParticleFlow_PFRecHitProducer_alpaka_TopologyAlpakaESData_h
#define RecoParticleFlow_PFRecHitProducer_alpaka_TopologyAlpakaESData_h

#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitTopologyAlpakaESData.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitTopologyAlpakaESDataSoA.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using PFRecHitHCALTopologyAlpakaESDataHost = reco::PFRecHitHCALTopologyAlpakaESDataHost;
  using PFRecHitHCALTopologyAlpakaESDataDevice = PortableCollection<reco::PFRecHitHCALTopologyAlpakaESDataSoA>;
  using PFRecHitECALTopologyAlpakaESDataHost = reco::PFRecHitECALTopologyAlpakaESDataHost;
  using PFRecHitECALTopologyAlpakaESDataDevice = PortableCollection<reco::PFRecHitECALTopologyAlpakaESDataSoA>;

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif
