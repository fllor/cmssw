#include <alpaka/alpaka.hpp>

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "RecoParticleFlow/PFRecHitProducer/interface/alpaka/PFRecHitProducerKernel.h"


namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace ParticleFlowRecHitProducerAlpaka;

  template<typename CAL>
  class PFRecHitProducerKernelImpl1 {
  public:
    template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(const TAcc& acc,
                                  const typename CAL::ParameterType::ConstView params,
                                  const typename CAL::TopologyType::ConstView topology,
                                  const CaloRecHitDeviceCollection::ConstView recHits, int32_t num_recHits,
                                  PFRecHitDeviceCollection::View pfRecHits,
                                  uint32_t* __restrict__ denseId2pfRecHit,
                                  uint32_t* __restrict__ num_pfRecHits) const {
      const int32_t num_blocks = alpaka::getWorkDiv<alpaka::Grid, alpaka::Blocks>(acc)[0u];

      // Strided loop over CaloRecHits
      for (int32_t i : cms::alpakatools::elements_with_stride(acc, num_recHits)) {
        // Check energy thresholds (specialised for HCAL/ECAL)
        if(!ApplyCuts(recHits[i], params))
          continue;

        // Use the appropriate synchronisation for the kernel layout
        const uint32_t j =
          (num_blocks == 1) ? alpaka::atomicAdd(acc, num_pfRecHits, 1u, alpaka::hierarchy::Blocks{})
                            : alpaka::atomicAdd(acc, num_pfRecHits, 1u, alpaka::hierarchy::Grids{});

        // Construct PFRecHit from CAL recHit (specialised for HCAL/ECAL)
        ConstructPFRecHit(pfRecHits[j], recHits[i]);

        // Fill denseId -> pfRecHit index map
        const uint32_t denseId = CAL::detId2denseId(pfRecHits[j].detId()) ;
        if(denseId <= CAL::SIZE)
          denseId2pfRecHit[denseId] = j;
        else
          printf("detId %u leads to invalid denseId %u. Allowed range [%u,%u)\n",
            pfRecHits[j].detId(), denseId, 0, CAL::SIZE);
      }
    }

    ALPAKA_FN_ACC static bool ApplyCuts(const CaloRecHitDeviceCollection::ConstView::const_element rh,
      const typename CAL::ParameterType::ConstView params);

    ALPAKA_FN_ACC static void ConstructPFRecHit(PFRecHitDeviceCollection::View::element pfrh,
      const CaloRecHitDeviceCollection::ConstView::const_element rh);
  };

  template<> ALPAKA_FN_ACC bool PFRecHitProducerKernelImpl1<HCAL>::ApplyCuts(
    const CaloRecHitDeviceCollection::ConstView::const_element rh,
    const PFRecHitHCALParamsAlpakaESDataDevice::ConstView params) {
    // Reject HCAL recHits below enery threshold
    float threshold = 9999.;
    const uint32_t detId = rh.detId();
    const uint32_t depth = HCAL::getDepth(detId);
    const uint32_t subdet = getSubdet(detId);
    if (subdet == HcalBarrel) {
      threshold = params.energyThresholds()[depth - 1];
    } else if (subdet == HcalEndcap) {
      threshold = params.energyThresholds()[depth - 1 + HCAL::maxDepthHB];
    } else {
      printf("Rechit with detId %u has invalid subdetector %u!\n", detId, subdet);
    }
    return rh.energy() >= threshold;
  }

  template<> ALPAKA_FN_ACC bool PFRecHitProducerKernelImpl1<ECAL>::ApplyCuts(
    const CaloRecHitDeviceCollection::ConstView::const_element rh,
    const PFRecHitECALParamsAlpakaESDataDevice::ConstView params) {
    // Reject ECAL recHits below energy threshold
    if(rh.energy() < params.energyThresholds()[ECAL::detId2denseId(rh.detId())])
      return false;

    // Reject ECAL recHits of bad quality
    if ((ECAL::checkFlag(rh.flags(), ECAL::Flags::kOutOfTime) && rh.energy() > 2)
      || ECAL::checkFlag(rh.flags(), ECAL::Flags::kTowerRecovered)
      || ECAL::checkFlag(rh.flags(), ECAL::Flags::kWeird)
      || ECAL::checkFlag(rh.flags(), ECAL::Flags::kDiWeird))
      return false;

    return true;
  }

  template<> ALPAKA_FN_ACC void PFRecHitProducerKernelImpl1<HCAL>::ConstructPFRecHit(
    PFRecHitDeviceCollection::View::element pfrh,
    const CaloRecHitDeviceCollection::ConstView::const_element rh) {
    pfrh.detId() = rh.detId();
    pfrh.energy() = rh.energy();
    pfrh.time() = rh.time();
    pfrh.depth() = HCAL::getDepth(pfrh.detId());
    const uint32_t subdet = getSubdet(pfrh.detId());
    if (subdet == HcalBarrel)
      pfrh.layer() = PFLayer::HCAL_BARREL1;
    else if (subdet == HcalEndcap)
      pfrh.layer() = PFLayer::HCAL_ENDCAP;
    else
      pfrh.layer() = PFLayer::NONE;
  }

  template<> ALPAKA_FN_ACC void PFRecHitProducerKernelImpl1<ECAL>::ConstructPFRecHit(
    PFRecHitDeviceCollection::View::element pfrh,
    const CaloRecHitDeviceCollection::ConstView::const_element rh) {
    pfrh.detId() = rh.detId();
    pfrh.energy() = rh.energy();
    pfrh.time() = rh.time();
    pfrh.depth() = 1;
    const uint32_t subdet = getSubdet(pfrh.detId());
    if (subdet == EcalBarrel)
      pfrh.layer() = PFLayer::ECAL_BARREL;
    else if (subdet == EcalEndcap)
      pfrh.layer() = PFLayer::ECAL_ENDCAP;
    else
      pfrh.layer() = PFLayer::NONE;
  }

  template<typename CAL>
  class PFRecHitProducerKernelImpl2 {
  public:
    template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(const TAcc& acc,
                                  const typename CAL::TopologyType::ConstView topology,
                                  const CaloRecHitDeviceCollection::ConstView recHits, int32_t num_recHits,
                                  PFRecHitDeviceCollection::View pfRecHits,
                                  const uint32_t* __restrict__ denseId2pfRecHit,
                                  uint32_t* __restrict__ num_pfRecHits) const {
      // First thread updates size field pfRecHits SoA
      if(const int32_t thread = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc)[0u]; thread == 0)
        pfRecHits.size() = *num_pfRecHits;

      // Assign position information and associate neighbours
      for(int32_t i : cms::alpakatools::elements_with_stride(acc, *num_pfRecHits)) {
        const uint32_t denseId = HCAL::detId2denseId(pfRecHits[i].detId());

        if constexpr(std::is_same_v<CAL,HCAL>) { // TODO ECAL topology
          pfRecHits[i].x() = topology[denseId].positionX();
          pfRecHits[i].y() = topology[denseId].positionY();
          pfRecHits[i].z() = topology[denseId].positionZ();
        }

        for(uint32_t n = 0; n < 8; n++)
        {
          pfRecHits[i].neighbours()(n) = -1;
          if constexpr(std::is_same_v<CAL,HCAL>) { // TODO ECAL topology
            const uint32_t denseId_neighbour = topology[denseId].neighbours()(n);
            if(denseId_neighbour != 0xffffffff)
            {
              const uint32_t pfRecHit_neighbour = denseId2pfRecHit[denseId_neighbour];
              if(pfRecHit_neighbour != 0xffffffff)
                pfRecHits[i].neighbours()(n) = (int32_t)pfRecHit_neighbour;
            }
          }
        }
      }
    }
  };

  template<typename CAL>
  PFRecHitProducerKernel<CAL>::PFRecHitProducerKernel(Queue& queue)
    : denseId2pfRecHit(cms::alpakatools::make_device_buffer<uint32_t[]>(queue, std::max(HCAL::SIZE, ECAL::SIZE))),
      num_pfRecHits(cms::alpakatools::make_device_buffer<uint32_t>(queue)) {
  }

  template<typename CAL>
  void PFRecHitProducerKernel<CAL>::execute(const Device& device, Queue& queue,
    const typename CAL::ParameterType& params,
    const typename CAL::TopologyType& topology,
    const CaloRecHitDeviceCollection& recHits,
    PFRecHitDeviceCollection& pfRecHits) {

    alpaka::memset(queue, denseId2pfRecHit, 0xff);  // Reset denseId -> pfRecHit index map
    alpaka::memset(queue, num_pfRecHits, 0x00);     // Reset global pfRecHit counter

    // Use only one block on the synchronous CPU backend, because there is no
    // performance gain in using multiple blocks, but there is a significant
    // penalty due to the more complex synchronisation.
    const uint32_t num_recHits = recHits->metadata().size();
    const uint32_t items = 64;
    const uint32_t groups = std::is_same_v<Device, alpaka::DevCpu> ? 1 : cms::alpakatools::divide_up_by(num_recHits, items);
    const auto work_div = cms::alpakatools::make_workdiv<Acc1D>(groups, items);

    alpaka::exec<Acc1D>(queue, work_div, PFRecHitProducerKernelImpl1<CAL>{},
      params.view(), topology.view(), recHits.view(), num_recHits, pfRecHits.view(), denseId2pfRecHit.data(), num_pfRecHits.data());
    alpaka::exec<Acc1D>(queue, work_div, PFRecHitProducerKernelImpl2<CAL>{},
      topology.view(), recHits.view(), num_recHits, pfRecHits.view(), denseId2pfRecHit.data(), num_pfRecHits.data());
  }

  // Instantiate templates
  template class PFRecHitProducerKernel<HCAL>;
  template class PFRecHitProducerKernel<ECAL>;
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE
