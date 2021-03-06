#include "TROOT.h"
#include "ReadDreamFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include <iostream>

void GetCorrelationsPhi(const char* filename, const char* Path, const char* prefix,
                        const char* addon = "") {
  //gStyle->SetOptStat(0);
  ReadDreamFile* DreamFile = new ReadDreamFile(3, 3);
  DreamFile->SetAnalysisFile(filename, Path, prefix, addon);

  DreamCF* CF_pp = new DreamCF();
  DreamPair* pp = new DreamPair("Part", 0.24, 0.34);
  DreamPair* ApAp = new DreamPair("AntiPart", 0.24, 0.34);

  DreamCF* CF_pPhi = new DreamCF();
  DreamPair* pPhi = new DreamPair("Part", 0.24, 0.34);
  DreamPair* ApPhi = new DreamPair("AntiPart", 0.24, 0.34);

  std::cout << "=========================" << std::endl;
  std::cout << "========Pair Set=========" << std::endl;
  std::cout << "=========================" << std::endl;
  pp->SetPair(DreamFile->GetPairDistributions(0, 0, ""));
  ApAp->SetPair(DreamFile->GetPairDistributions(1, 1, ""));

  pPhi->SetPair(DreamFile->GetPairDistributions(0, 2, ""));
  ApPhi->SetPair(DreamFile->GetPairDistributions(1, 2, ""));

  std::cout << "=========================" << std::endl;
  std::cout << "======Pair Shifted=======" << std::endl;
  std::cout << "=========================" << std::endl;
  pp->ShiftForEmpty(pp->GetPair());
  ApAp->ShiftForEmpty(ApAp->GetPair());

  pPhi->ShiftForEmpty(pPhi->GetPair());
  ApPhi->ShiftForEmpty(ApPhi->GetPair());

  std::cout << "=========================" << std::endl;
  std::cout << "====Pair Fix Shifted=====" << std::endl;
  std::cout << "=========================" << std::endl;
  pp->FixShift(pp->GetPairShiftedEmpty(0), ApAp->GetPairShiftedEmpty(0),
               ApAp->GetFirstBin());
  ApAp->FixShift(ApAp->GetPairShiftedEmpty(0), pp->GetPairShiftedEmpty(0),
                 pp->GetFirstBin());
  pp->FixShift(pp->GetPair(), ApAp->GetPair(), 0.004, true);
  ApAp->FixShift(ApAp->GetPair(), pp->GetPair(), 0.004, true);
  pPhi->FixShift(pPhi->GetPairShiftedEmpty(0), ApPhi->GetPairShiftedEmpty(0),
                 ApPhi->GetFirstBin());
  ApPhi->FixShift(ApPhi->GetPairShiftedEmpty(0), pPhi->GetPairShiftedEmpty(0),
                  pPhi->GetFirstBin());
  std::cout << "=========================" << std::endl;
  std::cout << "==Rebinning & Weighting==" << std::endl;
  std::cout << "=========================" << std::endl;

  std::vector<int> rebinVec = { { 4, 5 } };
  for (size_t iReb = 0; iReb < rebinVec.size(); ++iReb) {
    pPhi->Rebin(pPhi->GetPairFixShifted(0), rebinVec[iReb]);
    pPhi->ReweightMixedEvent(pPhi->GetPairRebinned(iReb), 0.2, 0.9);
    ApPhi->Rebin(ApPhi->GetPairFixShifted(0), rebinVec[iReb]);
    ApPhi->ReweightMixedEvent(ApPhi->GetPairRebinned(iReb), 0.2, 0.9);
  }
  pp->ReweightMixedEvent(pp->GetPairFixShifted(0), 0.2, 0.9);
  ApAp->ReweightMixedEvent(ApAp->GetPairFixShifted(0), 0.2, 0.9);

  pp->ReweightMixedEvent(pp->GetPairFixShifted(1), 0.2, 0.9);
  ApAp->ReweightMixedEvent(ApAp->GetPairFixShifted(1), 0.2, 0.9);

  std::cout << "=========================" << std::endl;
  std::cout << "=========CFs=============" << std::endl;
  std::cout << "=========================" << std::endl;

  TString foldername = filename;
  foldername.ReplaceAll("AnalysisResults.root", "");

  CF_pp->SetPairs(pp, ApAp);
  CF_pp->GetCorrelations();
  CF_pp->WriteOutput(Form("%sCFOutput_pp.root", foldername.Data()));

  CF_pPhi->SetPairs(pPhi, ApPhi);
  CF_pPhi->GetCorrelations();
  CF_pPhi->WriteOutput(Form("%sCFOutput_pPhi.root", foldername.Data()));
}
