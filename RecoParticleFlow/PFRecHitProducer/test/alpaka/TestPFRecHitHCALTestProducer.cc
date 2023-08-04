#include "DataFormats/PortableTestObjects/interface/alpaka/TestDeviceCollection.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/ESInputTag.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/EDProducer.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDPutToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESGetToken.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitParamsDeviceCollection.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitTopologyDeviceCollection.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitParamsRecord.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/PFRecHitTopologyRecord.h"

#include "TestAlgo.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class TestPFRecHitHCALTestProducer : public stream::EDProducer<> {
  public:
    TestPFRecHitHCALTestProducer(edm::ParameterSet const& config) :
      esParamsToken_{esConsumes(config.getParameter<edm::ESInputTag>("pfRecHitParams"))},
      esTopoToken_{esConsumes(config.getParameter<edm::ESInputTag>("pfRecHitTopology"))} {
      devicePutToken_ = produces("");
    }

    void produce(device::Event& iEvent, device::EventSetup const& iSetup) override {
      auto const& esParams = iSetup.getData(esParamsToken_);
      auto const& esTopo = iSetup.getData(esTopoToken_);

      auto deviceProduct = std::make_unique<portabletest::TestDeviceCollection>(256, iEvent.queue());

      algo_.printPFRecHitHCALESData(iEvent.queue(), esParams, esTopo);

      iEvent.put(devicePutToken_, std::move(deviceProduct));
    }

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::ESInputTag>("pfRecHitParams", edm::ESInputTag("pfRecHitHCALParamsESProducer", ""));
      desc.add<edm::ESInputTag>("pfRecHitTopology", edm::ESInputTag("pfRecHitHCALTopologyESProducer", ""));
      descriptions.addWithDefaultLabel(desc);
    }

  private:
    device::ESGetToken<PFRecHitHCALParamsDeviceCollection, PFRecHitHCALParamsRecord> const esParamsToken_;
    device::ESGetToken<PFRecHitHCALTopologyDeviceCollection, PFRecHitHCALTopologyRecord> const esTopoToken_;
    device::EDPutToken<portabletest::TestDeviceCollection> devicePutToken_;

    TestAlgo algo_;
  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(TestPFRecHitHCALTestProducer);
