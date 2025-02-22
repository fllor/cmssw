import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.nano_eras_cff import *
from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.triggerObjectTableProducer_cfi import triggerObjectTableProducer
from PhysicsTools.NanoAOD.globalVariablesTableProducer_cfi import globalVariablesTableProducer
import copy

unpackedPatTrigger = cms.EDProducer("PATTriggerObjectStandAloneUnpacker",
    patTriggerObjectsStandAlone = cms.InputTag('slimmedPatTrigger'),
    triggerResults              = cms.InputTag('TriggerResults::HLT'),
    unpackFilterLabels = cms.bool(True)
)

def mksel( selection, doc=None, bit=None):
    ddoc=""
    if 'OR' in selection:
        selection,ddoc=OR(selection)
    if 'AND' in selection:
        selection,ddoc=AND(selection)
    if type(selection)==list:
        selection,ddoc=OR(selection)
    if type(selection)==tuple:
        selection,ddoc=AND(selection)
    if doc==None:
        doc=ddoc
    #print("creating an entry with",selection,doc)
    return cms.PSet(selection=cms.string(selection),doc=cms.string(doc),bit=cms.uint32(bit)) if bit is not None else cms.PSet(selection=cms.string(selection),doc=cms.string(doc))

def chaintoken_(tokens,f,OR_or_AND=None):
    if (not type(tokens) in [list,tuple]):
        doc=tokens
        tokens= [t.strip().rstrip() for t in tokens.split(OR_or_AND)]
    else:
        doc=f' {OR_or_AND} '.join(tokens)
    selection = f"filter('{tokens[0]}')"
    for token in tokens[1:]:
        filter = f"filter('{token}')"
        selection = f"{f}({filter},{selection})"
    return selection,doc
def OR(tokens):
    return chaintoken_(tokens,"max","OR")
def AND(tokens):
    return chaintoken_(tokens,"min","AND")

triggerObjectTable = triggerObjectTableProducer.clone(
    name= cms.string("TrigObj"),
    src = cms.InputTag("unpackedPatTrigger"),
    l1EG = cms.InputTag("caloStage2Digis","EGamma"),
    l1Sum = cms.InputTag("caloStage2Digis","EtSum"),
    l1Jet = cms.InputTag("caloStage2Digis","Jet"),
    l1Muon = cms.InputTag("gmtStage2Digis","Muon"),
    l1Tau = cms.InputTag("caloStage2Digis","Tau"),
    selections = cms.VPSet(
        cms.PSet(
            name = cms.string("Electron (PixelMatched e/gamma)"), # this selects also photons for the moment!
            id = cms.int32(11),
            sel = cms.string("type(92) && pt > 7 && coll('hltEgammaCandidates') && filter('*PixelMatchFilter')"),
            l1seed = cms.string("type(-98)"),  l1deltaR = cms.double(0.3),
            #l2seed = cms.string("type(92) && coll('')"),  l2deltaR = cms.double(0.5),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel("filter('*CaloIdLTrackIdLIsoVL*TrackIso*Filter')","CaloIdL_TrackIdL_IsoVL"),
                mksel("filter('hltEle*WPTight*TrackIsoFilter*')","1e (WPTight)"),
                mksel("filter('hltEle*WPLoose*TrackIsoFilter')","1e (WPLoose)"),
                mksel("filter('*OverlapFilter*IsoEle*PFTau*')","OverlapFilter PFTau"),
                mksel("filter('hltEle*Ele*CaloIdLTrackIdLIsoVL*Filter')","2e"),
                mksel("filter('hltMu*TrkIsoVVL*Ele*CaloIdLTrackIdLIsoVL*Filter*')","1e-1mu"),
                mksel("filter('hlt*OverlapFilterIsoEle*PFTau*')","1e-1tau"),
                mksel("filter('hltEle*Ele*Ele*CaloIdLTrackIdLDphiLeg*Filter')","3e"),
                mksel(["hltL3fL1Mu*DoubleEG*Filtered*","hltMu*DiEle*CaloIdLTrackIdLElectronleg*Filter"],"2e-1mu"),
                mksel(["hltL3fL1DoubleMu*EG*Filter*","hltDiMu*Ele*CaloIdLTrackIdLElectronleg*Filter"],"1e-2mu"),
                mksel(("hltEle32L1DoubleEGWPTightGsfTrackIsoFilter","hltEGL1SingleEGOrFilter"),"1e (32_L1DoubleEG_AND_L1SingleEGOr)"),
                mksel("filter('hltEle*CaloIdVTGsfTrkIdTGsfDphiFilter')","1e (CaloIdVT_GsfTrkIdT)"),
                mksel("path('HLT_Ele*PFJet*')","1e (PFJet)"),
                mksel(["hltEG175HEFilter","hltEG200HEFilter"],"1e (Photon175_OR_Photon200)"),
                )
        ),
        cms.PSet(
            name = cms.string("Photon"),
            id = cms.int32(22),
            sel = cms.string("type(92) && pt > 20 && coll('hltEgammaCandidates')"),
            l1seed = cms.string("type(-98)"),  l1deltaR = cms.double(0.3),
            #l2seed = cms.string("type(92) && coll('')"),  l2deltaR = cms.double(0.5),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel("filter('hltEG33L1EG26HEFilter')","hltEG33L1EG26HEFilter"),
                mksel("filter('hltEG50HEFilter')","hltEG50HEFilter"),
                mksel("filter('hltEG75HEFilter')","hltEG75HEFilter"),
                mksel("filter('hltEG90HEFilter')","hltEG90HEFilter"),
                mksel("filter('hltEG120HEFilter')","hltEG120HEFilter"),
                mksel("filter('hltEG150HEFilter')","hltEG150HEFilter"),
                mksel("filter('hltEG175HEFilter')","hltEG150HEFilter"),
                mksel("filter('hltEG200HEFilter')","hltEG200HEFilter"),
                mksel("filter('hltHtEcal800')","hltHtEcal800"),
                mksel("filter('hltEG110EBTightIDTightIsoTrackIsoFilter')","hltEG110EBTightIDTightIsoTrackIsoFilter"),
                mksel("filter('hltEG120EBTightIDTightIsoTrackIsoFilter')","hltEG120EBTightIDTightIsoTrackIsoFilter"),
                mksel("filter('hltMu17Photon30IsoCaloIdPhotonlegTrackIsoFilter')","1mu-1photon")
            )
        ),
        cms.PSet(
            name = cms.string("Muon"),
            id = cms.int32(13),
            sel = cms.string("type(83) && pt > 5 && (coll('hltIterL3MuonCandidates') || (pt > 45 && coll('hltHighPtTkMuonCands')) || (pt > 95 && coll('hltOldL3MuonCandidates')))"),
            l1seed = cms.string("type(-81)"), l1deltaR = cms.double(0.5),
            l2seed = cms.string("type(83) && coll('hltL2MuonCandidates')"),  l2deltaR = cms.double(0.3),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel(["*RelTrkIsoVVLFiltered0p4","*RelTrkIsoVVLFiltered"],"TrkIsoVVL"),
                mksel(["hltL3crIso*IsoFiltered0p07","hltL3crIso*IsoFiltered0p08","hltL3crIso*IsoFiltered"],"Iso"),
                mksel("filter('*OverlapFilterIsoMu*PFTau*')","OverlapFilter PFTau"),
                mksel(["hltL3crIsoL1*SingleMu*IsoFiltered0p07","hltL3crIsoL1sMu*IsoFiltered0p07","hltL3crIsoL1*SingleMu*IsoFiltered0p08","hltL3crIsoL1sMu*IsoFiltered0p08","hltL3crIsoL1*SingleMu*IsoFiltered","hltL3crIsoL1sMu*IsoFiltered"],"1mu"),
                mksel("filter('hltDiMuon*Filtered*')","2mu"),
                mksel("filter('hltMu*TrkIsoVVL*Ele*CaloIdLTrackIdLIsoVL*Filter*')","1mu-1e"),
                mksel("filter('hlt*OverlapFilterIsoMu*PFTau*')","1mu-1tau"),
                mksel("filter('hltL3fL1TripleMu*')","3mu"),
                mksel(["hltL3fL1DoubleMu*EG*Filtered*","hltDiMu*Ele*CaloIdLTrackIdLElectronleg*Filter"],"2mu-1e"),
                mksel(["hltL3fL1Mu*DoubleEG*Filtered*","hltMu*DiEle*CaloIdLTrackIdLElectronleg*Filter"],"1mu-2e"),
                mksel(["hltL3fL1sMu*L3Filtered50*","hltL3fL1sMu*TkFiltered50*"],"1mu (Mu50)"),
                mksel(["hltL3fL1sMu*L3Filtered100*","hltL3fL1sMu*TkFiltered100*"],"1mu (Mu100)"),
                mksel("filter('hltMu17Photon30IsoCaloIdMuonlegL3Filtered17Q')","1mu-1photon")
            )
        ),
        cms.PSet(
            name = cms.string("Tau"),
            id = cms.int32(15),
            sel = cms.string("type(84) && pt > 5 && coll('*Tau*') && ( filter('*LooseChargedIso*') || filter('*MediumChargedIso*') || filter('*DeepTau*') || filter('*TightChargedIso*') || filter('*TightOOSCPhotons*') || filter('hltL2TauIsoFilter') || filter('*OverlapFilterIsoMu*') || filter('*OverlapFilterIsoEle*') || filter('*L1HLTMatched*') || filter('*Dz02*') || filter('*DoublePFTau*') || filter('*SinglePFTau*') || filter('hlt*SelectedPFTau') || filter('*DisplPFTau*') )"), #All trigger objects from a Tau collection + passing at least one filter
            l1seed = cms.string("type(-100)"), l1deltaR = cms.double(0.3),
            l2seed = cms.string("type(84) && coll('hltL2TauJetsL1IsoTauSeeded')"),  l2deltaR = cms.double(0.3),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel("filter('*LooseChargedIso*')","LooseChargedIso"),
                mksel("filter('*MediumChargedIso*')","MediumChargedIso"),
                mksel("filter('*TightChargedIso*')","TightChargedIso"),
                mksel("filter('*DeepTau*')","DeepTau"),
                mksel("filter('*TightOOSCPhotons*')","TightID OOSC photons"),
                mksel("filter('*Hps*')","HPS"),
                mksel("filter('hlt*DoublePFTau*TrackPt1*ChargedIsolation*Dz02*')","charged iso di-tau"),
                mksel("filter('hlt*DoublePFTau*DeepTau*L1HLTMatched')","deeptau di-tau"),
                mksel("filter('hlt*OverlapFilterIsoEle*WPTightGsf*PFTau*')","e-tau"),
                mksel("filter('hlt*OverlapFilterIsoMu*PFTau*')","mu-tau"),
                mksel("filter('hlt*SelectedPFTau*L1HLTMatched')","single-tau/tau+MET"),
                mksel("filter('hlt*DoublePFTau*TrackPt1*ChargedIso*')","run 2 VBF+ditau"),
                mksel("filter('hlt*DoublePFTau*Track*ChargedIso*AgainstMuon')","run 3 VBF+ditau"),
                mksel("filter('hltHpsSinglePFTau*HLTMatched')","run 3 double PF jets + ditau"),
                mksel("filter('hltHpsOverlapFilterDeepTauDoublePFTau*PFJet*')","di-tau + PFJet"),
                mksel("filter('hlt*Double*ChargedIsoDisplPFTau*Dxy*')","Displaced Tau"),
                mksel("filter('*Monitoring')","Monitoring"),
                mksel("filter('*Reg')","regional paths"),
                mksel("filter('*L1Seeded')","L1 seeded paths"),
                mksel("filter('*1Prong')","1 prong tau paths")
            )
        ),
        cms.PSet(
            name = cms.string("BoostedTau"),
            id = cms.int32(1515),
            sel = cms.string("type(85) && pt > 120 && coll('hltAK8PFJetsCorrected') && filter('hltAK8SinglePFJets*SoftDropMass40*ParticleNetTauTau')"),
            l1seed = cms.string("type(-99)"), l1deltaR = cms.double(0.3),
            l2seed = cms.string("type(85)  && coll('hltAK8CaloJetsCorrectedIDPassed')"),  l2deltaR = cms.double(0.3),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel("filter('hltAK8SinglePFJets*SoftDropMass40*ParticleNetTauTau')","HLT_AK8PFJetX_SoftDropMass40_PFAK8ParticleNetTauTau0p30")
            )
        ),
        cms.PSet(
            name = cms.string("Jet"),
            id = cms.int32(1),
            sel = cms.string("( type(0) || type(85) || type(86) || type(-99) )"),
            l1seed = cms.string("type(-99)"), l1deltaR = cms.double(0.3),
            l2seed = cms.string("type(85) || type(86) || type(-99)"),  l2deltaR = cms.double(0.3),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel(["hlt4PixelOnlyPFCentralJetTightIDPt20"]),
                mksel(["hlt3PixelOnlyPFCentralJetTightIDPt30"]),
                mksel(["hltPFJetFilterTwoC30"]),
                mksel(["hlt4PFCentralJetTightIDPt30"]),
                mksel(["hlt4PFCentralJetTightIDPt35"]),
                mksel(["hltQuadCentralJet30"]),
                mksel(["hlt2PixelOnlyPFCentralJetTightIDPt40"]),
                mksel(["hltL1sTripleJet1008572VBFIorHTTIorDoubleJetCIorSingleJet","hltL1sTripleJet1058576VBFIorHTTIorDoubleJetCIorSingleJet","hltL1sTripleJetVBFIorHTTIorSingleJet"]),
                mksel(["hlt3PFCentralJetTightIDPt40"]),
                mksel(["hlt3PFCentralJetTightIDPt45"]),
                mksel(["hltL1sQuadJetC60IorHTT380IorHTT280QuadJetIorHTT300QuadJet","hltL1sQuadJetC50to60IorHTT280to500IorHTT250to340QuadJet"]),
                mksel(["hltBTagCaloDeepCSVp17Double"]),
                mksel(["hltPFCentralJetLooseIDQuad30"]),
                mksel(["hlt1PFCentralJetLooseID75"]),
                mksel(["hlt2PFCentralJetLooseID60"]),
                mksel(["hlt3PFCentralJetLooseID45"]),
                mksel(["hlt4PFCentralJetLooseID40"]),
                mksel(["hltBTagPFDeepCSV4p5Triple"]),
                mksel("filter('hltHpsOverlapFilterDeepTauDoublePFTau*PFJet*')","(Double tau + jet) hltHpsOverlapFilterDeepTauDoublePFTau*PFJet*"),
                mksel("filter('*CrossCleaned*MediumDeepTauDitauWPPFTau*')","(VBF cross-cleaned from medium deeptau PFTau) *CrossCleaned*MediumDeepTauDitauWPPFTau*"),
                mksel("filter('*CrossCleanedUsingDiJetCorrChecker*')","(VBF cross-cleaned using dijet correlation checker) *CrossCleanedUsingDiJetCorrChecker*"),
                mksel("filter('hltHpsOverlapFilterDeepTauPFTau*PFJet*')","(monitoring muon + tau + jet)  hltHpsOverlapFilterDeepTauPFTau*PFJet*"),
                mksel(["hlt2PFCentralJetTightIDPt50"]),
                mksel(["hlt1PixelOnlyPFCentralJetTightIDPt60"]),
                mksel(["hlt1PFCentralJetTightIDPt70"]),
                mksel(["hltBTagPFDeepJet1p5Single"]),
                mksel(["hltBTagPFDeepJet4p5Triple"]),
                mksel(["hltBTagCentralJetPt35PFParticleNet2BTagSum0p65","hltBTagCentralJetPt30PFParticleNet2BTagSum0p65","hltPFJetTwoC30PFBTagParticleNet2BTagSum0p65"]),
                mksel(["hltBTagPFDeepCSV1p5Single"])
            ),
        ),
        cms.PSet(
            name = cms.string("FatJet"),
            id = cms.int32(6),
            sel = cms.string("type(85) && pt > 120 && coll('hltAK8PFJetsCorrected')"),
            l1seed = cms.string("type(-99)"), l1deltaR = cms.double(0.3),
            l2seed = cms.string("type(85)  && coll('hltAK8CaloJetsCorrectedIDPassed')"),  l2deltaR = cms.double(0.3),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet()
        ),
        cms.PSet(
            name = cms.string("MET"),
            id = cms.int32(2),
            sel = cms.string("type(87) && pt > 30 && coll('hltPFMETProducer')"),
            l1seed = cms.string("type(-87) && coll('L1ETM')"), l1deltaR = cms.double(9999),
            l1seed_2 = cms.string("type(-87) && coll('L1ETMHF')"), l1deltaR_2 = cms.double(9999),
            l2seed = cms.string("type( 87) && coll('hltMetClean')"),  l2deltaR = cms.double(9999),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet()
        ),
        cms.PSet(
            name = cms.string("HT"),
            id = cms.int32(3),
            sel = cms.string("type(89) || type(-89)"),
            l1seed = cms.string("type(-89) && coll('L1HTT')"), l1deltaR = cms.double(9999),
            l1seed_2 = cms.string("type(-89) && coll('L1HTTHF')"), l1deltaR_2 = cms.double(9999),
            l2seed = cms.string("type(89) && coll('hltHtMhtJet30')"),  l2deltaR = cms.double(9999),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel(["hltL1sTripleJetVBFIorHTTIorDoubleJetCIorSingleJet"]),
                mksel(["hltL1sQuadJetC50IorQuadJetC60IorHTT280IorHTT300IorHTT320IorTripleJet846848VBFIorTripleJet887256VBFIorTripleJet927664VBF","hltL1sQuadJetCIorTripleJetVBFIorHTT"]),
                mksel(["hltL1sQuadJetC60IorHTT380IorHTT280QuadJetIorHTT300QuadJet","hltL1sQuadJetC50to60IorHTT280to500IorHTT250to340QuadJet"]),
                mksel(["hltCaloQuadJet30HT300","hltCaloQuadJet30HT320"]),
                mksel(["hltPFCentralJetsLooseIDQuad30HT300","hltPFCentralJetsLooseIDQuad30HT330"])
            ),
        ),
        cms.PSet(
            name = cms.string("MHT"),
            id = cms.int32(4),
            sel = cms.string("type(90)"),
            l1seed = cms.string("type(-90) && coll('L1HTM')"), l1deltaR = cms.double(9999),
            l1seed_2 = cms.string("type(-90) && coll('L1HTMHF')"), l1deltaR_2 = cms.double(9999),
            l2seed = cms.string("type(90) && coll('hltHtMhtJet30')"),  l2deltaR = cms.double(9999),
            skipObjectsNotPassingQualityBits = cms.bool(True),
            qualityBits = cms.VPSet(
                mksel(["hltCaloQuadJet30HT300","hltCaloQuadJet30HT320"]),
                mksel(["hltPFCentralJetsLooseIDQuad30HT300","hltPFCentralJetsLooseIDQuad30HT330"])
            ),
        ),
    ),
)

# ERA-dependent configuration
# Tune filter and collection names to 2016 HLT menus
# FIXME: check non-lepton objects and cross check leptons
selections2016 = copy.deepcopy(triggerObjectTable.selections)
for sel in selections2016:
    if sel.name=='Muon':
        sel.sel = cms.string("type(83) && pt > 5 && (coll('hlt*L3MuonCandidates') || coll('hlt*TkMuonCands') || coll('hlt*TrkMuonCands'))")
        sel.qualityBits = cms.VPSet(
            mksel("filter('*RelTrkIso*Filtered0p4')","TrkIsoVVL"),
            mksel("filter('hltL3cr*IsoFiltered0p09')","Iso"),
            mksel("filter('*OverlapFilter*IsoMu*PFTau*')","OverlapFilter PFTau"),
            mksel("filter('hltL3f*IsoFiltered0p09')","IsoTkMu"),
            mksel(["hltL3fL1sMu*L3Filtered50*","hltL3fL1sMu*TkFiltered50*"],"1mu (Mu50)", bit=10)
        )
    elif sel.name=='Tau':
        sel.sel = cms.string("type(84) && pt > 5 && coll('*Tau*') && ( filter('*LooseIso*') || filter('*MediumIso*') || filter('*MediumComb*Iso*') || filter('hltL2TauIsoFilter') || filter('*OverlapFilter*IsoMu*') || filter('*OverlapFilter*IsoEle*') || filter('*L1HLTMatched*') || filter('*Dz02*') )")
        sel.qualityBits = cms.VPSet(
            mksel("(filter('*LooseIso*')-filter('*VLooseIso*'))","LooseIso"),
            mksel("filter('*Medium*Iso*')","Medium(Comb)Iso"),
            mksel("filter('*VLooseIso*')","VLooseIso"),
            mksel("0","None"),
            mksel("filter('hltL2TauIsoFilter')","L2p5 pixel iso"),
            mksel("filter('*OverlapFilter*IsoMu*')","OverlapFilter IsoMu"),
            mksel("filter('*OverlapFilter*IsoEle*')","OverlapFilter IsoEle"),
            mksel("filter('*L1HLTMatched*')","L1-HLT matched"),
            mksel("filter('*Dz02*')","Dz")
        )

run2_HLTconditions_2016.toModify(
  triggerObjectTable,
  selections = selections2016
)

from PhysicsTools.PatUtils.L1PrefiringWeightProducer_cff import prefiringweight
#Next lines are for UL2016 maps
(run2_muon_2016 & tracker_apv_vfp30_2016).toModify(
    prefiringweight,
    DataEraECAL = cms.string("UL2016preVFP"),
    DataEraMuon = cms.string("2016preVFP")
)
(run2_muon_2016 & ~tracker_apv_vfp30_2016).toModify(
    prefiringweight,
    DataEraECAL = cms.string("UL2016postVFP"),
    DataEraMuon = cms.string("2016postVFP")
)
#Next line is for UL2017 maps 
run2_jme_2017.toModify(
    prefiringweight,
    DataEraECAL = cms.string("UL2017BtoF"),
    DataEraMuon = cms.string("20172018")
)
#Next line is for UL2018 maps
run2_muon_2018.toModify(
    prefiringweight,
    DataEraECAL = cms.string("None"),
    DataEraMuon = cms.string("20172018")
)

l1PreFiringEventWeightTable = globalVariablesTableProducer.clone(
    name = cms.string("L1PreFiringWeight"),
    variables = cms.PSet(
        Nom = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProb"), "float", doc = "L1 pre-firing event correction weight (1-probability)", precision=8),
        Up = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbUp"), "float", doc = "L1 pre-firing event correction weight (1-probability), up var.", precision=8),
        Dn = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbDown"), "float", doc = "L1 pre-firing event correction weight (1-probability), down var.", precision=8),
        Muon_Nom = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbMuon"), "float", doc = "Muon L1 pre-firing event correction weight (1-probability)", precision=8),
        Muon_SystUp = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbMuonSystUp"), "float", doc = "Muon L1 pre-firing event correction weight (1-probability), up var. syst.", precision=8),
        Muon_SystDn = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbMuonSystDown"), "float", doc = "Muon L1 pre-firing event correction weight (1-probability), down var. syst.", precision=8),
        Muon_StatUp = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbMuonStatUp"), "float", doc = "Muon L1 pre-firing event correction weight (1-probability), up var. stat.", precision=8),
        Muon_StatDn = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbMuonStatDown"), "float", doc = "Muon L1 pre-firing event correction weight (1-probability), down var. stat.", precision=8),
        ECAL_Nom = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbECAL"), "float", doc = "ECAL L1 pre-firing event correction weight (1-probability)", precision=8),
        ECAL_Up = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbECALUp"), "float", doc = "ECAL L1 pre-firing event correction weight (1-probability), up var.", precision=8),
        ECAL_Dn = ExtVar(cms.InputTag("prefiringweight:nonPrefiringProbECALDown"), "float", doc = "ECAL L1 pre-firing event correction weight (1-probability), down var.", precision=8),
    )
)

l1bits=cms.EDProducer("L1TriggerResultsConverter",
                       src=cms.InputTag("gtStage2Digis"),
                       legacyL1=cms.bool(False),
                       storeUnprefireableBit=cms.bool(True),
                       src_ext=cms.InputTag("simGtExtUnprefireable"))

triggerObjectTablesTask = cms.Task( unpackedPatTrigger,triggerObjectTable,l1bits)

(run2_HLTconditions_2016 | run2_HLTconditions_2017 | run2_HLTconditions_2018).toReplaceWith(
    triggerObjectTablesTask, triggerObjectTablesTask.copyAndAdd(prefiringweight,l1PreFiringEventWeightTable)
)
