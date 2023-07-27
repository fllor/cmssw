#include "HeterogeneousCore/AlpakaCore/interface/alpaka/global/EDProducer.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/ParticleFlowReco_Alpaka/interface/CaloRecHitHostCollection.h"
#include "DataFormats/ParticleFlowReco_Alpaka/interface/alpaka/CaloRecHitDeviceCollection.h"

#define DEBUG false

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  template<typename RecHit>
  class CaloRecHitSoAProducer : public global::EDProducer<> {
  public:
    CaloRecHitSoAProducer(edm::ParameterSet const& config) :
      recHitsToken(consumes(config.getParameter<edm::InputTag>("src"))),
      deviceToken(produces()),
      synchronise(config.getParameter<bool>("synchronise"))
    {}

    void produce(edm::StreamID sid, device::Event& event, device::EventSetup const&) const override {
      const edm::SortedCollection<RecHit>& recHits = event.get(recHitsToken);
      const int32_t num_recHits = recHits.size();
      if(DEBUG)
        printf("Found %d recHits\n", num_recHits);

      reco::CaloRecHitHostCollection hostProduct{num_recHits, event.queue()};
      auto& view = hostProduct.view();

      for(int i = 0; i < num_recHits; i++)
      {
        ConvertRecHit(view[i], recHits[i]);

        if (DEBUG && i < 10)
          printf("recHit %4d %u %f %f %08x\n", i, view[i].detId(), view[i].energy(), view[i].time(), view[i].flags());
      }

      CaloRecHitDeviceCollection deviceProduct{num_recHits, event.queue()};
      alpaka::memcpy(event.queue(), deviceProduct.buffer(), hostProduct.buffer());
      if(synchronise)
        alpaka::wait(event.queue());
      event.emplace(deviceToken, std::move(deviceProduct));
    }

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("src");
      desc.add<bool>("synchronise", false);
      descriptions.addWithDefaultLabel(desc);
    }

  private:
    const edm::EDGetTokenT<edm::SortedCollection<RecHit>> recHitsToken;
    const device::EDPutToken<CaloRecHitDeviceCollection> deviceToken;
    const bool synchronise;

    static void ConvertRecHit(reco::CaloRecHitHostCollection::View::element to, const RecHit& from);
  };

  template<>
  void CaloRecHitSoAProducer<HBHERecHit>::ConvertRecHit(reco::CaloRecHitHostCollection::View::element to, const HBHERecHit& from)
  {
    // Fill SoA from HCAL rec hit
    to.detId()  = from.id().rawId();
    to.energy() = from.energy();
    to.time()   = from.time();
    to.flags()  = from.flags();
  }

  template<>
  void CaloRecHitSoAProducer<EcalRecHit>::ConvertRecHit(reco::CaloRecHitHostCollection::View::element to, const EcalRecHit& from)
  {
    // Fill SoA from ECAL rec hit
    to.detId()  = from.id().rawId();
    to.energy() = from.energy();
    to.time()   = from.time();
    to.flags()  = from.flagsBits();
  }

  using HCaloRecHitSoAProducer = CaloRecHitSoAProducer<HBHERecHit>;
  using ECaloRecHitSoAProducer = CaloRecHitSoAProducer<EcalRecHit>;
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(HCaloRecHitSoAProducer);
DEFINE_FWK_ALPAKA_MODULE(ECaloRecHitSoAProducer);
