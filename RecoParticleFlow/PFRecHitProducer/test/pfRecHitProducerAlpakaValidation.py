# Auto generated configuration file
# using:
# Revision: 1.19
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v
# with command line options: reHLT --processName reHLT -s HLT:@relval2021 --conditions auto:phase1_2021_realistic --datatier GEN-SIM-DIGI-RAW -n 5 --eventcontent FEVTDEBUGHLT --geometry DB:Extended --era Run3 --customise=HLTrigger/Configuration/customizeHLTforPatatrack.customizeHLTforPatatrack --filein /store/relval/CMSSW_12_3_0_pre5/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/123X_mcRun3_2021_realistic_v6-v1/10000/2639d8f2-aaa6-4a78-b7c2-9100a6717e6c.root
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_cff import Run3

process = cms.Process('rereHLT',Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('HLTrigger.Configuration.HLT_GRun_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load('Configuration.StandardSequences.Accelerators_cff')
process.load('HeterogeneousCore.AlpakaCore.ProcessAcceleratorAlpaka_cfi')

process.maxEvents = cms.untracked.PSet(
    #input = cms.untracked.int32(5),
    input = cms.untracked.int32(100),
    #input = cms.untracked.int32(1000),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('/store/relval/CMSSW_13_0_0/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU_130X_mcRun3_2022_realistic_v2_HS-v4/2590000/0088b51b-0cda-40f2-95fc-590f446624ee.root'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(
    FailPath = cms.untracked.vstring(),
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    SkipEvent = cms.untracked.vstring(),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    makeTriggerResults = cms.obsolete.untracked.bool,
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('reHLT nevts:5'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-DIGI-RAW'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('reHLT_HLT.root'),
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from HLTrigger.Configuration.CustomConfigs import ProcessName
process = ProcessName(process)

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2022_realistic', '')

# Path and EndPath definitions
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)

# Schedule definition
# process.schedule imported from cff in HLTrigger.Configuration
process.schedule.extend([process.endjob_step,process.FEVTDEBUGHLToutput_step])
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from HLTrigger.Configuration.customizeHLTforPatatrack
#from HLTrigger.Configuration.customizeHLTforPatatrack import customizeHLTforPatatrack, customiseCommon, customiseHcalLocalReconstruction

# only enable Hcal GPU
#process = customiseCommon(process)
#process = customiseHcalLocalReconstruction(process)

#call to customisation function customizeHLTforPatatrack imported from HLTrigger.Configuration.customizeHLTforPatatrack
#process = customizeHLTforPatatrack(process)

# Automatic addition of the customisation function from HLTrigger.Configuration.customizeHLTforMC
from HLTrigger.Configuration.customizeHLTforMC import customizeHLTforMC

#call to customisation function customizeHLTforMC imported from HLTrigger.Configuration.customizeHLTforMC
process = customizeHLTforMC(process)

# End of customisation functions


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

process.load( "HLTrigger.Timer.FastTimerService_cfi" )
if 'MessageLogger' in process.__dict__:
    process.MessageLogger.TriggerSummaryProducerAOD = cms.untracked.PSet()
    process.MessageLogger.L1GtTrigReport = cms.untracked.PSet()
    process.MessageLogger.L1TGlobalSummary = cms.untracked.PSet()
    process.MessageLogger.HLTrigReport = cms.untracked.PSet()
    process.MessageLogger.FastReport = cms.untracked.PSet()
    process.MessageLogger.ThroughputService = cms.untracked.PSet()
    process.MessageLogger.cerr.FastReport = cms.untracked.PSet( limit = cms.untracked.int32( 10000000 ) )





import sys
import argparse
parser = argparse.ArgumentParser(prog=sys.argv[0], description='Test and validation of PFRecHitProducer with Alpaka')
parser.add_argument('-c', '--cal', type=str, default='HCAL',
                    help='Calorimeter type. Possible options: HCAL, ECAL. Default: HCAL')
parser.add_argument('-b', '--backend', type=str, default='auto',
                    help='Alpaka backend. Possible options: CPU, GPU, auto. Default: auto')
parser.add_argument('-s', '--synchronise', action='store_true', default=False,
                    help='Put synchronisation point at the end of Alpaka modules (for benchmarking performance)')
parser.add_argument('-t', '--threads', type=int, default=8,
                    help='Number of threads. Default: 8')
parser.add_argument('-d', '--debug', type=int, default=0, const=1, nargs="?",
                    help='Dump PFRecHits for first event (n>0) or first error (n<0). This applies to the n-th validation (1: Legacy vs Alpaka, 2: Legacy vs Legacy-from-Alpaka, 3: Alpaka vs Legacy-from-Alpaka). Default: 0')
args = parser.parse_args(sys.argv[3:])

if(args.debug and args.threads != 1):
    args.threads = 1
    print("Number of threads set to 1 for debugging")

assert args.cal.lower() in ["hcal", "ecal", "h", "e"], "Invalid calorimeter type"
hcal = args.cal.lower() in ["hcal", "h"]

alpaka_backends = {
    "cpu": "alpaka_serial_sync::%s",  # Execute on CPU
    "gpu": "alpaka_cuda_async::%s",   # Execute using CUDA
    "cuda": "alpaka_cuda_async::%s",  # Execute using CUDA
    "auto": "%s@alpaka"               # Let framework choose
}
assert args.backend.lower() in alpaka_backends, "Invalid backend"
alpaka_backend_str = alpaka_backends[args.backend.lower()]



#####################################
##    Legacy PFRecHit producer     ##
#####################################
if hcal:
    process.hltParticleFlowRecHit = cms.EDProducer("PFRecHitProducer",
        navigator = cms.PSet(
            hcalEnums = cms.vint32(1, 2),
            name = cms.string('PFRecHitHCALDenseIdNavigator')
        ),
        producers = cms.VPSet(cms.PSet(
            name = cms.string('PFHBHERecHitCreator'),
            qualityTests = cms.VPSet(
                cms.PSet(
                    cuts = cms.VPSet(
                        cms.PSet(
                            depth = cms.vint32(1, 2, 3, 4),
                            detectorEnum = cms.int32(1),
                            threshold = cms.vdouble(0.1, 0.2, 0.3, 0.3)
                        ),
                        cms.PSet(
                            depth = cms.vint32(1, 2, 3, 4, 5, 6, 7),
                            detectorEnum = cms.int32(2),
                            threshold = cms.vdouble(0.1, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2)
                        )
                    ),
                    name = cms.string('PFRecHitQTestHCALThresholdVsDepth')
                ),
                cms.PSet(
                    cleaningThresholds = cms.vdouble(0.0),
                    flags = cms.vstring('Standard'),
                    maxSeverities = cms.vint32(11),
                    name = cms.string('PFRecHitQTestHCALChannel')
                )
            ),
            src = cms.InputTag("hltHbhereco")
        ))
    )
else:  # ecal
    qualityTestsECAL = cms.VPSet(
        cms.PSet(
            name = cms.string("PFRecHitQTestDBThreshold"),
            applySelectionsToAllCrystals=cms.bool(True),
        ),
        cms.PSet(
            name = cms.string("PFRecHitQTestECAL"),
            cleaningThreshold = cms.double(2.0),
            timingCleaning = cms.bool(True),
            topologicalCleaning = cms.bool(True),
            skipTTRecoveredHits = cms.bool(True)
        )
    )
    process.hltParticleFlowRecHit = cms.EDProducer("PFRecHitProducer",
        navigator = cms.PSet(
            name = cms.string("PFRecHitECALNavigator"),
            barrel = cms.PSet( ),
            endcap = cms.PSet( )
        ),
        producers = cms.VPSet(
            cms.PSet(
                name = cms.string("PFEBRecHitCreator"),
                src  = cms.InputTag("hltEcalRecHit","EcalRecHitsEB"),
                srFlags = cms.InputTag(""),
                qualityTests = qualityTestsECAL
            ),
            cms.PSet(
                name = cms.string("PFEERecHitCreator"),
                src  = cms.InputTag("hltEcalRecHit","EcalRecHitsEE"),
                srFlags = cms.InputTag(""),
                qualityTests = qualityTestsECAL
            )
        )
    )


#####################################
##    Alpaka PFRecHit producer     ##
#####################################
# Convert legacy CaloRecHits to CaloRecHitSoA
if hcal:
    process.hltParticleFlowRecHitToSoA = cms.EDProducer(alpaka_backend_str % "HCALRecHitSoAProducer",
        src = cms.InputTag("hltHbhereco"),
        synchronise = cms.bool(args.synchronise)
    )
else:  # ecal
    process.hltParticleFlowRecHitEBToSoA = cms.EDProducer(alpaka_backend_str % "ECALRecHitSoAProducer",
        src = cms.InputTag("hltEcalRecHit","EcalRecHitsEB"),
        synchronise = cms.bool(args.synchronise)
    )
    process.hltParticleFlowRecHitEEToSoA = cms.EDProducer(alpaka_backend_str % "ECALRecHitSoAProducer",
        src = cms.InputTag("hltEcalRecHit","EcalRecHitsEE"),
        synchronise = cms.bool(args.synchronise)
    )

# Construct PFRecHitsSoA
process.jobConfAlpakaRcdESSource = cms.ESSource('EmptyESSource',
    recordName = cms.string('PFRecHitHCALParamsRecord'),
    iovIsRunNotTime = cms.bool(True),
    firstValid = cms.vuint32(1)
)
process.pfRecHitHCALTopologyAlpakaESRcdESSource = cms.ESSource('EmptyESSource',
  recordName = cms.string('PFRecHitHCALTopologyRecord'),
  iovIsRunNotTime = cms.bool(True),
  firstValid = cms.vuint32(1)
)
if hcal:
    process.hltParticleFlowRecHitParamsESProducer = cms.ESProducer(alpaka_backend_str % "PFRecHitHCALParamsESProducer",
        energyThresholdsHB = cms.vdouble( 0.1, 0.2, 0.3, 0.3 ),
        energyThresholdsHE = cms.vdouble( 0.1, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 )
    )
    process.hltParticleFlowRecHitTopologyESProducer = cms.ESProducer(alpaka_backend_str % "PFRecHitHCALTopologyESProducer")
    process.hltParticleFlowPFRecHitAlpaka = cms.EDProducer(alpaka_backend_str % "PFRecHitProducerAlpakaHCAL",
        producers = cms.VPSet(
            cms.PSet(
                src = cms.InputTag("hltParticleFlowRecHitToSoA"),
                params = cms.ESInputTag("hltParticleFlowRecHitParamsESProducer:"),
            )
        ),
        topology = cms.ESInputTag("hltParticleFlowRecHitTopologyESProducer:"),
        synchronise = cms.bool(args.synchronise)
    )
else:  # ecal
    process.hltParticleFlowRecHitParamsESProducer = cms.ESProducer(alpaka_backend_str % "PFRecHitECALParamsESProducer")
    process.hltParticleFlowRecHitTopologyESProducer = cms.ESProducer(alpaka_backend_str % "PFRecHitECALTopologyESProducer")
    process.hltParticleFlowPFRecHitAlpaka = cms.EDProducer(alpaka_backend_str % "PFRecHitProducerAlpakaECAL",
        producers = cms.VPSet(
            cms.PSet(
                src = cms.InputTag("hltParticleFlowRecHitEBToSoA"),
                params = cms.ESInputTag("hltParticleFlowRecHitParamsESProducer:")
            ),
            cms.PSet(
                src = cms.InputTag("hltParticleFlowRecHitEEToSoA"),
                params = cms.ESInputTag("hltParticleFlowRecHitParamsESProducer:")
            )
        ),
        topology = cms.ESInputTag("hltParticleFlowRecHitTopologyESProducer:"),
        synchronise = cms.bool(args.synchronise)
    )

# Convert Alpaka PFRecHits to legacy format (for validation)
process.htlParticleFlowAlpakaToLegacyPFRecHits = cms.EDProducer("LegacyPFRecHitProducer",
    src = cms.InputTag("hltParticleFlowPFRecHitAlpaka")
)


#####################################
##       PFRecHit validation       ##
#####################################
# Validate legacy format from legacy module vs SoA format from Alpaka module
# This is the main Alpaka vs legacy test
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
process.hltParticleFlowPFRecHitComparison = DQMEDAnalyzer("PFRecHitProducerTest",
    #caloRecHits = cms.untracked.InputTag("hltParticleFlowRecHitToSoA"),
    pfRecHitsSource1 = cms.untracked.InputTag("hltParticleFlowRecHit"),
    pfRecHitsSource2 = cms.untracked.InputTag("hltParticleFlowPFRecHitAlpaka"),
    pfRecHitsType1 = cms.untracked.string("legacy"),
    pfRecHitsType2 = cms.untracked.string("alpaka"),
    title = cms.untracked.string("Legacy vs Alpaka"),
    dumpFirstEvent = cms.untracked.bool(args.debug == 1),
    dumpFirstError = cms.untracked.bool(args.debug == -1)
)

# Validate legacy format from legacy module vs legacy format from Alpaka module
process.htlParticleFlowAlpakaToLegacyPFRecHitsComparison1 = DQMEDAnalyzer("PFRecHitProducerTest",
    pfRecHitsSource1 = cms.untracked.InputTag("hltParticleFlowRecHit"),
    pfRecHitsSource2 = cms.untracked.InputTag("htlParticleFlowAlpakaToLegacyPFRecHits"),
    pfRecHitsType1 = cms.untracked.string("legacy"),
    pfRecHitsType2 = cms.untracked.string("legacy"),
    title = cms.untracked.string("Legacy vs Legacy-from-Alpaka"),
    dumpFirstEvent = cms.untracked.bool(args.debug == 2),
    dumpFirstError = cms.untracked.bool(args.debug == -2)
)

# Validate SoA format from Alpaka module vs legacy format from Alpaka module
# This tests the SoA-to-legacy conversion module
process.htlParticleFlowAlpakaToLegacyPFRecHitsComparison2 = DQMEDAnalyzer("PFRecHitProducerTest",
    pfRecHitsSource1 = cms.untracked.InputTag("hltParticleFlowPFRecHitAlpaka"),
    pfRecHitsSource2 = cms.untracked.InputTag("htlParticleFlowAlpakaToLegacyPFRecHits"),
    pfRecHitsType1 = cms.untracked.string("alpaka"),
    pfRecHitsType2 = cms.untracked.string("legacy"),
    title = cms.untracked.string("Alpaka vs Legacy-from-Alpaka"),
    dumpFirstEvent = cms.untracked.bool(args.debug == 3),
    dumpFirstError = cms.untracked.bool(args.debug == -3)
)

# Additional customization
process.FEVTDEBUGHLToutput.outputCommands = cms.untracked.vstring('drop  *_*_*_*')
process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*ParticleFlow*HBHE*_*_*')
process.FEVTDEBUGHLToutput.outputCommands.append('keep *_hltParticleFlowRecHitToSoA_*_*')
process.FEVTDEBUGHLToutput.outputCommands.append('keep *_hltParticleFlowPFRecHitAlpaka_*_*')

# Path/sequence definitions
path = process.hltParticleFlowRecHit           # Construct PFRecHits on CPU
if hcal:
    path += process.hltParticleFlowRecHitToSoA     # Convert legacy CaloRecHits to SoA and copy to device, HCAL barrel+endcap
else:  # ecal
    path += process.hltParticleFlowRecHitEBToSoA   # Convert legacy CaloRecHits to SoA and copy to device, ECAL barrel
    path += process.hltParticleFlowRecHitEEToSoA   # Convert legacy CaloRecHits to SoA and copy to device, ECAL endcap
path += process.hltParticleFlowPFRecHitAlpaka  # Construct PFRecHits on device
path += process.hltParticleFlowPFRecHitComparison  # Validate Alpaka vs CPU
if hcal:
    path += process.htlParticleFlowAlpakaToLegacyPFRecHits             # Convert Alpaka PFRecHits to legacy format
    path += process.htlParticleFlowAlpakaToLegacyPFRecHitsComparison1  # Validate legacy-format-from-alpaka vs regular legacy format
    path += process.htlParticleFlowAlpakaToLegacyPFRecHitsComparison2  # Validate Alpaka format vs legacy-format-from-alpaka

process.HBHEPFCPUGPUTask = cms.Path(path)
process.schedule = cms.Schedule(process.HBHEPFCPUGPUTask)
process.schedule.extend([process.endjob_step,process.FEVTDEBUGHLToutput_step])

process.options.numberOfThreads = cms.untracked.uint32(args.threads)
