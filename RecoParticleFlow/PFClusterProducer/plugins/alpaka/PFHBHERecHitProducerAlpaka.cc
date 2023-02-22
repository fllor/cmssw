#include <memory>

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/Event.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EventSetup.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/EDProducer.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "CUDADataFormats/Common/interface/Product.h"
#include "CUDADataFormats/HcalRecHitSoA/interface/RecHitCollection.h"
#include "CUDADataFormats/PFRecHitSoA/interface/PFRecHitCollection.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
//#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/RunningAverage.h"
#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitCreatorBase.h"
#include "RecoParticleFlow/PFClusterProducer/interface/PFRecHitNavigatorBase.h"


namespace cms::alpakatools {
  template <>
  struct CopyToHost<reco::PFRecHitCollection> {
    template <typename TQueue>
    static reco::PFRecHitCollection copyAsync(TQueue& queue, reco::PFRecHitCollection const& deviceProduct) {
      printf("Copying reco::PFRecHitCollection to host\n");
      return deviceProduct;
    }
  };
}


namespace ALPAKA_ACCELERATOR_NAMESPACE {

class PFHBHERecHitProducerAlpaka final : public stream::EDProducer<> {
public:
  explicit PFHBHERecHitProducerAlpaka(const edm::ParameterSet& iConfig);
  ~PFHBHERecHitProducerAlpaka() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(device::Event&, const device::EventSetup&) override;
  //void beginRun(edm::Run const&, const device::EventSetup&) override;
  std::vector<std::unique_ptr<PFRecHitCreatorBase>> creators_;
  std::unique_ptr<PFRecHitNavigatorBase> navigator_;

  using IProductType = cms::cuda::Product<hcal::RecHitCollection<calo::common::DevStoragePolicy>>;
  const edm::EDGetTokenT<IProductType> recHitToken;
  const device::EDPutToken<reco::PFRecHitCollection> pfRecHitToken, pfRecHitTokenCleaned;
};

#include "HeterogeneousCore/AlpakaCore/interface/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(PFHBHERecHitProducerAlpaka);

namespace {
  bool sortByDetId(const reco::PFRecHit& a, const reco::PFRecHit& b) { return a.detId() < b.detId(); }

  edm::RunningAverage localRA1;
  edm::RunningAverage localRA2;
}  // namespace

PFHBHERecHitProducerAlpaka::PFHBHERecHitProducerAlpaka(const edm::ParameterSet& iConfig) :
  recHitToken(consumes<IProductType>(iConfig.getParameter<std::string>("src"))),
  pfRecHitToken(produces()),
  pfRecHitTokenCleaned(produces("Cleaned")) {
  //produces<reco::PFRecHitCollection>();
  //produces<reco::PFRecHitCollection>("Cleaned");

  //edm::ConsumesCollector cc = consumesCollector();
  ////for (edm::ParameterSet& creator : iConfig.getParameter<std::vector<edm::ParameterSet>>("producers"))
  ////  creators_.emplace_back(PFRecHitFactory::get()->create(creator.getParameter<std::string>("name"), creator, cc));
  ////edm::ParameterSet navSet = iConfig.getParameter<edm::ParameterSet>("navigator");
  ////navigator_ = PFRecHitNavigationFactory::get()->create(navSet.getParameter<std::string>("name"), navSet, cc);
}

PFHBHERecHitProducerAlpaka::~PFHBHERecHitProducerAlpaka() = default;

//void PFHBHERecHitProducerAlpaka::beginRun(edm::Run const& iRun, const device::EventSetup& iSetup) {
//  for (const auto& creator : creators_) {
//    creator->init(iSetup);
//  }
//  navigator_->init(iSetup);
//}

// ------------ method called to produce the data  ------------
void PFHBHERecHitProducerAlpaka::produce(device::Event& iEvent, const device::EventSetup& iSetup) {
  using namespace edm;
  auto out = std::make_unique<reco::PFRecHitCollection>();
  auto cleaned = std::make_unique<reco::PFRecHitCollection>();

  out->reserve(localRA1.upper());
  cleaned->reserve(localRA2.upper());

//  for (const auto& creator : creators_) {
//    creator->importRecHits(out, cleaned, iEvent, iSetup);
//  }

  if (out->capacity() > 2 * out->size())
    out->shrink_to_fit();
  if (cleaned->capacity() > 2 * cleaned->size())
    cleaned->shrink_to_fit();
  localRA1.update(out->size());
  localRA2.update(cleaned->size());
  std::sort(out->begin(), out->end(), sortByDetId);

  //create a refprod here
//  edm::RefProd<reco::PFRecHitCollection> refProd = iEvent.getRefBeforePut<reco::PFRecHitCollection>();
//
//  for (auto& pfrechit : *out) {
//    navigator_->associateNeighbours(pfrechit, out, refProd);
//  }
//
//  iEvent.put(std::move(out), "");
//  iEvent.put(std::move(cleaned), "Cleaned");
}

void PFHBHERecHitProducerAlpaka::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  //desc.setUnknown();
  desc.setAllowAnything();

  descriptions.addWithDefaultLabel(desc);
}

} // namespace ALPAKA_ACCELERATOR_NAMESPACE
