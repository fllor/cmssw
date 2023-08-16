#ifndef RecoParticleFlow_PFRecHitProducer_test_alpaka_TestAlgo_h
#define RecoParticleFlow_PFRecHitProducer_test_alpaka_TestAlgo_h

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitTopologyDeviceCollection.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitParamsDeviceCollection.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class TestAlgo {
  public:
    void printPFRecHitHCALESData(Queue& queue,
                                 PFRecHitHCALParamsDeviceCollection const& esParams,
                                 PFRecHitHCALTopologyDeviceCollection const& esTopo) const;
  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif  // RecoParticleFlow_PFRecHitProducer_plugins_alpaka_TestAlgo_h
