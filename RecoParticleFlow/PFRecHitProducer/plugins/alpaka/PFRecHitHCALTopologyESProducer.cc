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

      auto product = std::make_unique<PFRecHitHCALTopologyAlpakaESDataHost>(HCAL::SIZE, cms::alpakatools::host());
      auto view = product->view();

      for (auto hcalSubdet : {HcalBarrel, HcalEndcap})
        for (auto const detId : geom.getValidDetIds(DetId::Hcal, hcalSubdet)) {
          const uint32_t denseId = HCAL::detId2denseId(detId);

          const GlobalPoint pos = hcalGeo.at(detId.subdetId())->getGeometry(detId)->getPosition();
          view[denseId].positionX() = pos.x();
          view[denseId].positionY() = pos.y();
          view[denseId].positionZ() = pos.z();

          for (uint32_t n = 0; n < 8; n++) {
            uint32_t neighDetId = GetNeighbourDetId(detId, n, topo);
            if (HCAL::IsValidDetId(neighDetId))
              view[denseId].neighbours()(n) = HCAL::detId2denseId(neighDetId);
            else
              view[denseId].neighbours()(n) = 0xffffffff;
          }
        }

      // Remove neighbours that are not backward compatible
      for (auto hcalSubdet : {HcalBarrel, HcalEndcap})
        for (auto const detId : geom.getValidDetIds(DetId::Hcal, hcalSubdet)) {
          const uint32_t denseId = HCAL::detId2denseId(detId);
          for (uint32_t n = 0; n < 8; n++) {
            const reco::PFRecHitsTopologyNeighbours& neighboursOfNeighbour = view[view[denseId].neighbours()[n]].neighbours();
            if(std::find(neighboursOfNeighbour.begin(), neighboursOfNeighbour.end(), denseId) == neighboursOfNeighbour.end())
              view[denseId].neighbours()[n] = 0xffffffff;
          }
        }

      for (auto hcalSubdet : {HcalBarrel, HcalEndcap})
        for (auto const detId : geom.getValidDetIds(DetId::Hcal, hcalSubdet)) {
          logDebug() << "detId: rawId=" << detId
                          << " subdet=" << detId.subdetId()
                        << " denseId=" << denseId;
          logDebug() << "  position: x=" << view[denseId].positionX()
                                << " y=" << view[denseId].positionY()
                                << " z=" << view[denseId].positionZ();
          for (uint32_t idx = 0; idx < 8; ++idx) {
            logDebug() << "  neighbour[" << idx << "]: " << view[denseId].neighbours()(idx);
          }
        }

      return product;
    }

    uint32_t GetNeighbourDetId(const uint32_t detId, const uint32_t direction, const HcalTopology& topo) {
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
        if (HCAL::getZside(detId) > 0)  // positive eta: east -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 2, topo), 0, topo);
        else  // negative eta: move in phi first then move to east (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 0, topo), 2, topo);
      }
      if(direction == 5) {  // SOUTHWEST
        if (HCAL::getZside(detId) > 0)  // positive eta: move in phi first then move to west (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 1, topo), 3, topo);
        else  // negative eta: west -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 3, topo), 1, topo);
      }
      if(direction == 6) {  // SOUTHEAST
        if (HCAL::getZside(detId) > 0)  // positive eta: east -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 2, topo), 1, topo);
        else  // negative eta: move in phi first then move to east (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 1, topo), 2, topo);
      }
      if(direction == 7) {  // NORTHWEST
        if (HCAL::getZside(detId) > 0)  // positive eta: move in phi first then move to west (coarser phi granularity)
          return GetNeighbourDetId(GetNeighbourDetId(detId, 0, topo), 3, topo);
        else  // negative eta: west -> move to smaller |ieta| (finner phi granularity) first
          return GetNeighbourDetId(GetNeighbourDetId(detId, 3, topo), 0, topo);
      }
      return 0;
    }
  };
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

DEFINE_FWK_EVENTSETUP_ALPAKA_MODULE(PFRecHitHCALTopologyESProducer);
