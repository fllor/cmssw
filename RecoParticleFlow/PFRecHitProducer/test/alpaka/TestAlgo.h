#ifndef RecoParticleFlow_PFRecHitProducer_test_alpaka_TestAlgo_h
#define RecoParticleFlow_PFRecHitProducer_test_alpaka_TestAlgo_h

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitTopologyAlpakaESData.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitParamsAlpakaESData.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class TestAlgo {
  public:
    void printPFRecHitHCALESData(Queue& queue,
      PFRecHitHCALParamsAlpakaESDataDevice const& esParams, PFRecHitHCALTopologyAlpakaESDataDevice const& esTopo) const;
  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif  // RecoParticleFlow_PFRecHitProducer_plugins_alpaka_TestAlgo_h
