#ifndef RecoParticleFlow_PFRecHitProducer_PFRecHitParamsAlpakaESDataSoA_h
#define RecoParticleFlow_PFRecHitProducer_PFRecHitParamsAlpakaESDataSoA_h

#include <Eigen/Core>
#include <Eigen/Dense>

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"
#include "DataFormats/SoATemplate/interface/SoAView.h"

namespace reco {

  GENERATE_SOA_LAYOUT(PFRecHitHCALParamsAlpakaESDataSoALayout,
                      SOA_COLUMN(float, energyThresholds))

  using PFRecHitHCALParamsAlpakaESDataSoA = PFRecHitHCALParamsAlpakaESDataSoALayout<>;



  GENERATE_SOA_LAYOUT(PFRecHitECALParamsAlpakaESDataSoALayout,
                      SOA_COLUMN(float, energyThresholds))

  using PFRecHitECALParamsAlpakaESDataSoA = PFRecHitECALParamsAlpakaESDataSoALayout<>;

}

#endif
