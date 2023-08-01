#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
//#include "FWCore/Utilities/interface/Exception.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESProducer.h"
//#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
//#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/CalorimeterDefinitions.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"

#include "Geometry/CaloTopology/interface/HcalTopology.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "Geometry/CaloTopology/interface/EcalBarrelTopology.h"
#include "Geometry/CaloTopology/interface/EcalEndcapTopology.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <variant>

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace ParticleFlowRecHitProducerAlpaka;

  class PFRecHitECALTopologyESProducer : public ESProducer {
  public:
    PFRecHitECALTopologyESProducer(edm::ParameterSet const& iConfig) :
      ESProducer(iConfig) {
      auto cc = setWhatProduced(this);
      geomToken_ = cc.consumes();
    }

  private:
    std::vector<int> const hcalEnums_;
    edm::ESGetToken<CaloGeometry, CaloGeometryRecord> geomToken_;

    constexpr static const char* kProducerName = "PFRecHitECALTopologyESProducer";

    auto logDebug() const {
      return LogTrace(kProducerName) << "[" << kProducerName << "] ";
    }

  public:
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<std::string>("appendToDataLabel", "");
      descriptions.addWithDefaultLabel(desc);
    }

    std::unique_ptr<PFRecHitECALTopologyAlpakaESDataHost> produce(const PFRecHitECALTopologyRecord& iRecord) {
      auto const& geom = iRecord.get(geomToken_);

      std::vector<int> ecalEnums = {EcalSubdetector::EcalBarrel, EcalSubdetector::EcalEndcap};
      //std::unordered_map<int, const CaloSubdetectorGeometry*> ecalGeo;
      //for(const auto subdet : ecalEnums)
      //  ecalGeo[subdet] = geom.getSubdetectorGeometry(DetId::Detector::Ecal, subdet);

      auto product = std::make_unique<PFRecHitECALTopologyAlpakaESDataHost>(ECAL::SIZE, cms::alpakatools::host());
      auto view = product->view();

      for(const auto subdet : ecalEnums)
      {
        const CaloSubdetectorGeometry* geo = geom.getSubdetectorGeometry(DetId::Detector::Ecal, subdet);
        std::variant<EcalBarrelTopology, EcalEndcapTopology> topoVar;
        if(subdet == EcalSubdetector::EcalBarrel)
          topoVar.emplace<EcalBarrelTopology>(geom);
        else
          topoVar.emplace<EcalEndcapTopology>(geom);
        const CaloSubdetectorTopology& topo = std::visit([](const auto& t) -> const CaloSubdetectorTopology& {return t;}, topoVar);

        for (auto const detId : geom.getValidDetIds(DetId::Detector::Ecal, subdet)) {
          const uint32_t denseId = ECAL::detId2denseId(detId);

          const GlobalPoint pos = geo->getGeometry(detId)->getPosition();
          view[denseId].positionX() = pos.x();
          view[denseId].positionY() = pos.y();
          view[denseId].positionZ() = pos.z();

          for (uint32_t n = 0; n < 8; n++) {
            uint32_t neighDetId = GetNeighbourDetIdEE(detId, n, topo);
            if (ECAL::IsValidDetId(neighDetId))
              view[denseId].neighbours()(n) = ECAL::detId2denseId(neighDetId);
            else
              view[denseId].neighbours()(n) = 0xffffffff;
          }
        }
      }

      // Remove neighbours that are not backward compatible
      //for(const auto subdet : ecalEnums)
      //  for (auto const detId : geom.getValidDetIds(DetId::Detector::Ecal, subdet)) {
      //    const uint32_t denseId = ECAL::detId2denseId(detId);
      //    for (uint32_t n = 0; n < 8; n++) {
      //      const reco::PFRecHitsTopologyNeighbours& neighboursOfNeighbour = view[view[denseId].neighbours()[n]].neighbours();
      //      if(std::find(neighboursOfNeighbour.begin(), neighboursOfNeighbour.end(), denseId) == neighboursOfNeighbour.end())
      //        view[denseId].neighbours()[n] = 0xffffffff;
      //    }
      //  }

      //for(const auto subdet : ecalEnums)
      //  for (const auto detId : geom.getValidDetIds(DetId::Detector::Ecal, subdet)) {
      //    const uint32_t denseId = ECAL::detId2denseId(detId);
      //    printf("PFRecHitECALTopologyESProducer: detId:%u denseId:%u pos:%f,%f,%f\n",
      //      (uint32_t)detId, denseId,
      //      view[denseId].positionX(), view[denseId].positionY(), view[denseId].positionZ()
      //    );
      //    //for (uint32_t idx = 0; idx < 8; ++idx) {
      //    //  logDebug() << "  neighbour[" << idx << "]: " << view[denseId].neighbours()(idx);
      //    //}
      //  }

      return product;
    }

    uint32_t GetNeighbourDetIdEE(const uint32_t detId, const uint32_t direction, const CaloSubdetectorTopology& topo) {
      // desired order for PF: NORTH, SOUTH, EAST, WEST, NORTHEAST, SOUTHWEST, SOUTHEAST, NORTHWEST
      if(detId == 0)
        return 0;

      if(direction == 0)  // NORTH
        return topo.goNorth(detId);  // larger iphi values (except phi boundary)
      if(direction == 1)  // SOUTH
        return topo.goSouth(detId);  // smaller iphi values (except phi boundary)
      if(direction == 2)  // EAST
        return topo.goEast(detId);  // smaller ieta values
      if(direction == 3)  // WEST
        return topo.goWest(detId);  // larger ieta values

      if(direction == 4) { // NORTHEAST
        const uint32_t NE = GetNeighbourDetId(GetNeighbourDetId(detId, 0, topo), 2, topo);
        if(NE)
          return NE;
        return GetNeighbourDetId(GetNeighbourDetId(detId, 2, topo), 0, topo);
      }
      if(direction == 5) {  // SOUTHWEST
        const uint32_t SW = GetNeighbourDetId(GetNeighbourDetId(detId, 1, topo), 3, topo);
        if(SW)
          return SW;
        return GetNeighbourDetId(GetNeighbourDetId(detId, 3, topo), 1, topo);
      }
      if(direction == 6) {  // SOUTHEAST
        const uint32_t ES = GetNeighbourDetId(GetNeighbourDetId(detId, 2, topo), 1, topo);
        if(ES)
          return ES;
        return GetNeighbourDetId(GetNeighbourDetId(detId, 1, topo), 2, topo);
      }
      if(direction == 7) {  // NORTHWEST
        const uint32_t WN =  GetNeighbourDetId(GetNeighbourDetId(detId, 3, topo), 0, topo);
        if(WN)
          return WN;
        return GetNeighbourDetId(GetNeighbourDetId(detId, 0, topo), 3, topo);
      }
      return 0;
    }

    uint32_t GetNeighbourDetId(const uint32_t detId, const uint32_t direction, const CaloSubdetectorTopology& topo) {
      // desired order for PF: NORTH, SOUTH, EAST, WEST, NORTHEAST, SOUTHWEST, SOUTHEAST, NORTHWEST
      if(detId == 0)
        return 0;

      if(direction == 0)  // NORTH
        return topo.goNorth(detId);  // larger iphi values (except phi boundary)
      if(direction == 1)  // SOUTH
        return topo.goSouth(detId);  // smaller iphi values (except phi boundary)
      if(direction == 2)  // EAST
        return topo.goEast(detId);  // smaller ieta values
      if(direction == 3)  // WEST
        return topo.goWest(detId);  // larger ieta values

      if(direction == 4) { // NORTHEAST
        if (ECAL::getZside(detId) > 0)  // positive eta: east -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 2, topo), 0, topo);
        else  // negative eta: move in phi first then move to east (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 0, topo), 2, topo);
      }
      if(direction == 5) {  // SOUTHWEST
        if (ECAL::getZside(detId) > 0)  // positive eta: move in phi first then move to west (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 1, topo), 3, topo);
        else  // negative eta: west -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 3, topo), 1, topo);
      }
      if(direction == 6) {  // SOUTHEAST
        if (ECAL::getZside(detId) > 0)  // positive eta: east -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 2, topo), 1, topo);
        else  // negative eta: move in phi first then move to east (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 1, topo), 2, topo);
      }
      if(direction == 7) {  // NORTHWEST
        if (ECAL::getZside(detId) > 0)  // positive eta: move in phi first then move to west (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 0, topo), 3, topo);
        else  // negative eta: west -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 3, topo), 0, topo);
      }
      return 0;
    }
  };
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ModuleFactory.h"
DEFINE_FWK_EVENTSETUP_ALPAKA_MODULE(PFRecHitECALTopologyESProducer);
