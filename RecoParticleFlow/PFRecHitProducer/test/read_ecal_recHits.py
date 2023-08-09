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
    input = cms.untracked.int32(1300),
    #input = cms.untracked.int32(10000),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring('file:/data/user/florkows/ecal_recHits.root'),
    fileNames = cms.untracked.vstring('file:/data/user/florkows/ecal_recHits_uncompressed.root'),
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
    fileName = cms.untracked.string('hcal_recHits_processed.root'),
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
parser = argparse.ArgumentParser(prog=sys.argv[0], description='Throughput test of PFRecHitProducer with Alpaka')
parser.add_argument('-b', '--backend', type=str, default='cpu',
                    help='Alpaka backend. Possible options: legacy, CPU, GPU, auto. Default: CPU')
parser.add_argument('-s', '--synchronise', action='store_true', default=False,
                    help='Put synchronisation point at the end of Alpaka modules (for benchmarking performance)')
parser.add_argument('-t', '--threads', type=int, default=8,
                    help='Number of threads. Default: 8')
parser.add_argument('-i', '--iterations', type=int, default=1,
                    help='How many times to run PFRecHitProducer module (for benchmarking). Default: 1')
parser.add_argument('-e', '--events', type=int, default=process.maxEvents.input.value(),
                    help='Number of events to process. Default: %d' % process.maxEvents.input.value())
args = parser.parse_args(sys.argv[sys.argv.index("--")+1:] if "--" in sys.argv else [])
backend = args.backend.lower()
process.maxEvents.input = args.events

#####################################
##    Legacy PFRecHit producer     ##
#####################################
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
process.hltParticleFlowRecHitECAL = cms.EDProducer("PFRecHitProducer",
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
if backend == "legacy":
    pass
elif backend == "cpu":
    alpaka_backend_str = "alpaka_serial_sync::%s"   # Execute on CPU
elif backend == "gpu" or backend == "cuda":
    alpaka_backend_str = "alpaka_cuda_async::%s"    # Execute using CUDA
elif backend == "auto":
    alpaka_backend_str = "%s@alpaka"                # Let framework choose
else:
    print("Invalid backend:", backend)
    sys.exit(1)
print("Selected backend:", backend)

if backend != "legacy":
    # Convert legacy CaloRecHits to CaloRecHitSoA
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
        recordName = cms.string('PFRecHitECALParamsRecord'),
        iovIsRunNotTime = cms.bool(True),
        firstValid = cms.vuint32(1)
    )
    process.pfRecHitECALTopologyAlpakaESRcdESSource = cms.ESSource('EmptyESSource',
    recordName = cms.string('PFRecHitECALTopologyRecord'),
    iovIsRunNotTime = cms.bool(True),
    firstValid = cms.vuint32(1)
    )
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

#
# Additional customization
process.FEVTDEBUGHLToutput.outputCommands = cms.untracked.vstring('drop *_*_*_*')
#process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*ParticleFlow*HBHE*_*_*')
#process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*HbherecoLegacy*_*_*')
##process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*HbherecoFromGPU*_*_*')
#process.FEVTDEBUGHLToutput.outputCommands.append('keep *_*Hbhereco*_*_*')
#process.FEVTDEBUGHLToutput.outputCommands.append('keep *_hltParticleFlowRecHitToSoA_*_*')
#process.FEVTDEBUGHLToutput.outputCommands.append('keep *_hltParticleFlowPFRecHitAlpaka_*_*')

#
# Run only localreco, PFRecHit and PFCluster producers for HBHE only
#process.source.fileNames = cms.untracked.vstring('file:/cms/data/hatake/ana/PF/GPU/CMSSW_12_4_0_v2/src/test/v21/GPU/reHLT_HLT.root ')

# Path/sequence definitions
if backend == "legacy":
    path = process.hltParticleFlowRecHitECAL
    for i in range(1, args.iterations):
        n = "hltParticleFlowRecHitECAL%02d" % i
        setattr(process, n, process.hltParticleFlowRecHitECAL.clone())
        path += getattr(process, n)
    process.HBHEPFCPUGPUTask = cms.Path(path)
else:
    path = process.hltParticleFlowRecHitEBToSoA    # Convert legacy CaloRecHits to SoA and copy to device
    path += process.hltParticleFlowRecHitEEToSoA
    path += process.hltParticleFlowPFRecHitAlpaka  # Construct PFRecHits on device
    for i in range(1, args.iterations):
        n = "hltParticleFlowPFRecHitAlpaka%02d" % i
        setattr(process, n, process.hltParticleFlowPFRecHitAlpaka.clone())
        path += getattr(process, n)
    process.HBHEPFCPUGPUTask = cms.Path(path)
process.schedule = cms.Schedule(process.HBHEPFCPUGPUTask)
process.schedule.extend([process.endjob_step,process.FEVTDEBUGHLToutput_step])

process.options.numberOfThreads = cms.untracked.uint32(args.threads)
