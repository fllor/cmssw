#ifndef RecoParticleFlow_PFRecHitProducer_interface_PFRecHitTopologyAlpakaESData_h
#define RecoParticleFlow_PFRecHitProducer_interface_PFRecHitTopologyAlpakaESData_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "HeterogeneousCore/AlpakaInterface/interface/CopyToDevice.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitTopologyAlpakaESDataSoA.h"

namespace reco {

  using PFRecHitHCALTopologyAlpakaESDataHost = PortableHostCollection<PFRecHitHCALTopologyAlpakaESDataSoA>;

  using PFRecHitECALTopologyAlpakaESDataHost = PortableHostCollection<PFRecHitECALTopologyAlpakaESDataSoA>;

}

#endif
