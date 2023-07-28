#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/EDProducer.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/ParticleFlowReco_Alpaka/interface/alpaka/PFRecHitDeviceCollection.h"
#include "DataFormats/ParticleFlowReco_Alpaka/interface/alpaka/CaloRecHitDeviceCollection.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitParamsAlpakaESData.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitHBHETopologyAlpakaESData.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/JobConfigurationAlpakaRecord.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitHBHETopologyAlpakaESRcd.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitProducerKernel.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/CalorimeterDefinitions.h"

#define DEBUG false

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace  ParticleFlowRecHitProducerAlpaka;

  template<typename CAL>
  class PFRecHitProducerAlpaka : public stream::EDProducer<> {
  public:
    PFRecHitProducerAlpaka(edm::ParameterSet const& config) :
      paramsToken(esConsumes(config.getParameter<edm::ESInputTag>("params"))),
      topologyToken(esConsumes(config.getParameter<edm::ESInputTag>("topology"))),
      recHitsToken(consumes(config.getParameter<edm::InputTag>("src"))),
      pfRecHitsToken(produces()),
      synchronise(config.getParameter<bool>("synchronise"))
    {}

    void produce(device::Event& event, device::EventSetup const& setup) override {
      const typename CAL::ParameterType& params = setup.getData(paramsToken);
      const PFRecHitHBHETopologyAlpakaESDataDevice& topology = setup.getData(topologyToken);
      const CaloRecHitDeviceCollection& recHits = event.get(recHitsToken);
      const int num_recHits = recHits->metadata().size();
      PFRecHitDeviceCollection pfRecHits{num_recHits, event.queue()};

      if(!kernel)
        kernel.emplace(event.queue());
      kernel->execute(event.device(), event.queue(), params, topology, recHits, pfRecHits);

      if(synchronise)
        alpaka::wait(event.queue());

      event.emplace(pfRecHitsToken, std::move(pfRecHits));
    }

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("src");
      desc.add<edm::ESInputTag>("params");
      desc.add<edm::ESInputTag>("topology");
      desc.add<bool>("synchronise", false);
      descriptions.addWithDefaultLabel(desc);
    }

  private:
    const device::ESGetToken<typename CAL::ParameterType, JobConfigurationAlpakaRecord> paramsToken;
    const device::ESGetToken<PFRecHitHBHETopologyAlpakaESDataDevice, PFRecHitHBHETopologyAlpakaESRcd> topologyToken;
    const device::EDGetToken<CaloRecHitDeviceCollection> recHitsToken;
    const device::EDPutToken<PFRecHitDeviceCollection> pfRecHitsToken;
    const bool synchronise;
    std::optional<PFRecHitProducerKernel<CAL>> kernel = {};
  };

  using PFRecHitProducerAlpakaHCAL = PFRecHitProducerAlpaka<HCAL>;
  using PFRecHitProducerAlpakaECAL = PFRecHitProducerAlpaka<ECAL>;
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(PFRecHitProducerAlpakaHCAL);
DEFINE_FWK_ALPAKA_MODULE(PFRecHitProducerAlpakaECAL);
