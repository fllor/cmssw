#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFLayer.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFraction.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class RecHitSoAProducer : public edm::global::EDProducer<> {
public:
  RecHitSoAProducer(edm::ParameterSet const& conf);
  ~RecHitSoAProducer() override = default;
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
  //static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  const edm::EDGetTokenT<edm::SortedCollection<HBHERecHit>> recHitsToken;
};

RecHitSoAProducer::RecHitSoAProducer(const edm::ParameterSet& config)
 : recHitsToken(consumes<edm::SortedCollection<HBHERecHit>>(config.getParameter<edm::InputTag>("src")))
{}

void RecHitSoAProducer::produce(edm::StreamID, edm::Event& event, const edm::EventSetup&) const {
  edm::Handle<edm::SortedCollection<HBHERecHit>> recHits;
  event.getByToken(recHitsToken, recHits);
  fprintf(stderr, "Found %zd recHits\n", recHits->size());
  printf("Found %zd recHits\n", recHits->size());
  for(size_t i = 0; i < recHits->size(); i++)
  {
    const HBHERecHit& rh = recHits->operator[](i);
    printf("recHit %4lu %u\n", i, rh.id().rawId());
  }
}

// void PFRecHitCreatorGPUComparisonTask::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
//   edm::ParameterSetDescription desc;
//   //desc.setUnknown();
//   desc.add<edm::InputTag>("pfClusterToken_ref", edm::InputTag("particleFlowClusterHBHE"));
//   desc.add<edm::InputTag>("pfClusterToken_target", edm::InputTag("particleFlowClusterHBHEonGPU"));
//   desc.addUntracked<std::string>("pfCaloGPUCompDir", "pfClusterHBHEGPUv");
//   descriptions.addDefault(desc);
// }
  
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(RecHitSoAProducer);
