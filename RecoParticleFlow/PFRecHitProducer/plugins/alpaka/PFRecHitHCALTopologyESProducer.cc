#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/HcalTopology.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESProducer.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ModuleFactory.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "RecoParticleFlow/PFClusterProducer/interface/PFHCALDenseIdNavigatorCore.h"
#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/CalorimeterDefinitions.h"

#include <algorithm>
#include <memory>
#include <vector>

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace ParticleFlowRecHitProducerAlpaka;

  class PFRecHitHCALTopologyESProducer : public ESProducer {
  public:
    PFRecHitHCALTopologyESProducer(edm::ParameterSet const& iConfig) :
      ESProducer(iConfig) ,
      hcalEnums_(iConfig.getParameter<std::vector<int>>("hcalEnums")) {
      auto cc = setWhatProduced(this);
      hcalToken_ = cc.consumes();
      geomToken_ = cc.consumes();
    }

  private:
    std::vector<int> const hcalEnums_;
    edm::ESGetToken<HcalTopology, HcalRecNumberingRecord> hcalToken_;
    edm::ESGetToken<CaloGeometry, CaloGeometryRecord> geomToken_;

    constexpr static const char* kProducerName = "PFRecHitHCALTopologyESProducer";

    auto logDebug() const {
      return LogTrace(kProducerName) << "[" << kProducerName << "] ";
    }

  public:
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<std::string>("appendToDataLabel", "");
      desc.add<std::vector<int>>("hcalEnums", {1, 2});
      descriptions.addWithDefaultLabel(desc);
    }

    std::unique_ptr<PFRecHitHCALTopologyAlpakaESDataHost> produce(PFRecHitHCALTopologyRecord const& iRecord) {
      auto const& geom = iRecord.get(geomToken_);
      auto const& topo = iRecord.get(hcalToken_);

      std::unordered_map<int, const CaloSubdetectorGeometry*> hcalGeo;
      hcalGeo[HcalBarrel] = geom.getSubdetectorGeometry(DetId::Hcal, HcalBarrel);
      hcalGeo[HcalEndcap] = geom.getSubdetectorGeometry(DetId::Hcal, HcalEndcap);

      PFHCALDenseIdNavigatorCore navicore{hcalEnums_, geom, topo};

      // Check that denseIds are in expected range
      auto const& denseIds = navicore.getValidDenseIds();
      assert(*std::max_element(denseIds.begin(), denseIds.end()) < HCAL::SIZE);

      auto product = std::make_unique<PFRecHitHCALTopologyAlpakaESDataHost>(HCAL::SIZE, cms::alpakatools::host());
      auto view = product->view();

      for (auto hcalSubdet : {HcalBarrel, HcalEndcap})
      for (auto const detId : geom.getValidDetIds(DetId::Hcal, hcalSubdet)) {
        const uint32_t denseId = HCAL::detId2denseId(detId);
        const GlobalPoint pos = hcalGeo.at(detId.subdetId())->getGeometry(detId)->getPosition();

        view[denseId].positionX() = pos.x();
        view[denseId].positionY() = pos.y();
        view[denseId].positionZ() = pos.z();

        auto neigh = navicore.getNeighbours(denseId);
        // order from navigator: S, SE, SW, E,  W, NE, NW,  N
        // desired order for PF: N,  S,  E, W, NE, SW, SE, NW
        const uint32_t order[8] = {1, 6, 5, 2, 3, 4, 7, 0};

        for (uint32_t n = 0; n < 8; ++n) {
          view[denseId].neighbours()(order[n]) = 0xffffffff;

          // cmssdt.cern.ch/lxr/source/RecoParticleFlow/PFClusterProducer/interface/PFHCALDenseIdNavigator.h#0087
          // Order: CENTER(NONE),SOUTH,SOUTHEAST,SOUTHWEST,EAST,WEST,NORTHEAST,NORTHWEST,NORTH
          // neigh[0] is the rechit itself. Skip for neighbour array
          // If no neighbour exists in a direction, the value will be 0
          // Some neighbors from HF included! Need to test if these are included in the map!
          uint32_t neighDetId = neigh[n + 1];
          if (neighDetId > 0 && (getSubdet(neighDetId) == HcalBarrel || getSubdet(neighDetId) == HcalEndcap))
            view[denseId].neighbours()(order[n]) = HCAL::detId2denseId(neighDetId);
        }

        logDebug() << "detId: rawId=" << detId << " subdet=" << detId.subdetId() << " denseId=" << denseId;
        logDebug() << "  position: x=" << pos.x() << " y=" << pos.y() << " z=" << pos.z();
        for (uint32_t idx = 0; idx < 8; ++idx) {
          logDebug() << "  neighbour[" << idx << "]: " << view[denseId].neighbours()(idx);
        }
      }

      return product;
    }
  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

DEFINE_FWK_EVENTSETUP_ALPAKA_MODULE(PFRecHitHCALTopologyESProducer);
