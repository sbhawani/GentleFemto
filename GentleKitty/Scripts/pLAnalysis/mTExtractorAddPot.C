#include "VariationmTAnalysis.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLatex.h"


int main(int argc, char* argv[]) {
//  gROOT->ProcessLine("gErrorIgnoreLevel = 2001");
  const char* DataDir = argv[1];
  const char* FitDirResonance = argv[2];
  int SourceOption = atoi(argv[3]);
  const int nMTBins = atoi(argv[4]);
  const char* cutString = argv[5];
  const int outFileNumber = atoi(argv[6]);
  const char* AngFolder = argv[7]?argv[7]:"";

  TFile* out = nullptr;
  if (AngFolder != "") {
    out =
        TFile::Open(
            TString::Format("RadpLvsmT_%s_%u.root", AngFolder, outFileNumber)
                .Data(),
            "read");
  } else {
    out = TFile::Open(
        TString::Format("RadpLvsmT_%u.root", outFileNumber).Data(), "read");
  }
  if (out) {
    std::cout
        << "Already processed "
        << TString::Format("RadpLvsmT_%s_%u.root", AngFolder, outFileNumber)
            .Data()
        << "\n";
    return -1;
  }

  TString avgmTFile = TString::Format("%s/AveragemT.root", DataDir);
  TFile* mTFile = TFile::Open(avgmTFile, "READ");
  if (!mTFile) {
    std::cout << "No mT File \n";
    return -1;
  }
  TGraphErrors* avgmT = (TGraphErrors*) mTFile->Get("AveragemT_pLVar0");
  if (!avgmT) {
    std::cout << "no average mT graph " << std::endl;
    mTFile->ls();
    return -1;
  }
  VariationmTAnalysis* analyser = new VariationmTAnalysis(1);
  TCut AnalysisCut = "chiSqNDF<30";
  analyser->AppendAndCut(AnalysisCut);
  TCut UserCut = cutString;
  analyser->AppendAndCut(UserCut);
  analyser->SetHistName("hCk_RebinnedpLVar");
  analyser->SetFileName("OutFileVarpL.root");
  analyser->SetmTAverage(avgmT);
  analyser->SetLegData("p-#Lambda #oplus #bar{p}-#bar{#Lambda}", "fpe");
  analyser->SetLegModel("Usmani/#chi_{EFT} LO/NLO (fit)" , "l", 1, -1);
  if (SourceOption == 0) {
    analyser->SetSourceName("Gaussian Source");
  } else if (SourceOption == 1) {
    analyser->SetSourceName("Gaussian Source + Resonances");
  }
  analyser->SetTextXMin(0.35);
  analyser->SetPlottingRange(0, 230);
  //this implies a folder structure following mTBin_[1,2,3,....]
  for (int imt = 1; imt <= nMTBins; ++imt) {
    std::cout << "========================== \n";
    std::cout << "mTBin: " << imt << std::endl;
    std::cout << "========================== \n";
    TString mTDataDir = Form("%s/mTBin_%u", DataDir, imt);
    analyser->SetSystematic(mTDataDir.Data());
    TString mTFitDirResonance = Form("%s/mTBin_%u/", FitDirResonance, imt);
    analyser->SetVariation(mTFitDirResonance.Data(), 0);
  }

  if (nMTBins == 6) {
    std::vector<float> mTBins = { 1.08, 1.26, 1.32, 1.44, 1.65, 1.9, 4.5  };
    analyser->SetmTBins(mTBins);
  } else if (nMTBins == 3) {
    std::vector<float> mTBins = { 1.08, 1.3,1.5, 4.5 };
    analyser->SetmTBins(mTBins);
  }
  analyser->MakeCFPlotsSingleBand();
  if (AngFolder != "") {
    analyser->StoreRadvsmT(TString::Format("RadpLvsmT_%s_%u.root",AngFolder,outFileNumber), 0);
  } else {
    analyser->StoreRadvsmT(TString::Format("RadpLvsmT_%u.root",outFileNumber), 0);
  }

  return 1;
}

