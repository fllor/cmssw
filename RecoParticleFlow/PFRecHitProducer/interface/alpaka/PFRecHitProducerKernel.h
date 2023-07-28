#ifndef RecoParticleFlow_PFRecHitProducer_alpaka_PFRecHitProducerKernel_h
#define RecoParticleFlow_PFRecHitProducer_alpaka_PFRecHitProducerKernel_h

#include "DataFormats/ParticleFlowReco_Alpaka/interface/alpaka/PFRecHitDeviceCollection.h"
#include "DataFormats/ParticleFlowReco_Alpaka/interface/alpaka/CaloRecHitDeviceCollection.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitParamsAlpakaESData.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitHBHETopologyAlpakaESData.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/CalorimeterDefinitions.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  template<typename CAL>
  class PFRecHitProducerKernel {
  public:
    PFRecHitProducerKernel(Queue& queue);

    void execute(const Device& device, Queue& queue,
      const typename CAL::ParameterType& params,
      const PFRecHitHBHETopologyAlpakaESDataDevice& topology,
      const CaloRecHitDeviceCollection& recHits,
      PFRecHitDeviceCollection& collection);

  private:
    cms::alpakatools::device_buffer<Device, uint32_t[]> denseId2pfRecHit;
    cms::alpakatools::device_buffer<Device, uint32_t> num_pfRecHits;
  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif 
