#include "PeriodQA.h"
#include "DecayQA.h"
#include "TSystemDirectory.h"

PeriodQA::PeriodQA()
    : fDirectory(),
      fPeriods() {
}

void PeriodQA::SetDirectory(const char *dir) {
  fDirectory = dir;

  TSystemDirectory *workdir = new TSystemDirectory("workdir", dir);
  TList *list = workdir->GetListOfFiles();

  TIter next(list);
  TObject *obj = nullptr;
  while (obj = next()) {
    TString objName = obj->GetName();
    if (!objName.Contains("AnalysisResults"))
      continue;

    TString LHCperiod = objName;
    LHCperiod.ReplaceAll("AnalysisResults_", "");
    LHCperiod.ReplaceAll(".root", "");
    fPeriods.push_back(LHCperiod);
  }
  std::sort(fPeriods.begin(), fPeriods.end(),
            [](TString a, TString b) {return a<b;});
}

void PeriodQA::ProcessQA(const char* prefix, const char* addon) {
  auto histPurityLambda = PeriodQAHist("histQALambda", "Purity #Lambda (%)");
  auto histPurityAntiLambda = PeriodQAHist("histQAAntiLambda",
                                           "Purity #bar{#Lambda} (%)");

  int i = 0;
  for (auto it : fPeriods) {
    TString filename = fDirectory.Data();
    filename += "AnalysisResults_";
    filename += it;
    filename += ".root";
    ForgivingReader* reader = new ForgivingReader(filename.Data(), prefix,
                                                  addon);
    DecayQA* v0QA = new DecayQA("#Lambda", "p#pi");
    v0QA->SetDecayCuts(reader->Getv0Cuts());
    v0QA->SetCanvasDivisions(5, 2);
    v0QA->SetIMHistoScale(1.75, 0.8, 0.35);
    v0QA->SetRangesFitting(1.109, 1.121, 1.09, 1.15);
    v0QA->GetPeriodQA(1.112, 1.120, it.Data());
    float purity = v0QA->GetPurity();
    if (purity > 1E-6) {
      histPurityLambda->SetBinContent(i + 1, purity * 100.f);
    }

    DecayQA* antiv0QA = new DecayQA("#bar{#Lambda}", "p#pi");
    antiv0QA->SetDecayCuts(reader->GetAntiv0Cuts());
    antiv0QA->SetCanvasDivisions(5, 2);
    antiv0QA->SetIMHistoScale(1.75, 0.8, 0.35);
    antiv0QA->SetRangesFitting(1.109, 1.121, 1.09, 1.15);
    antiv0QA->GetPeriodQA(1.112, 1.120, it.Data());
    purity = antiv0QA->GetPurity();
    if (purity > 1E-6) {
      histPurityAntiLambda->SetBinContent(i + 1, purity * 100.f);
    }
    delete v0QA;
    delete antiv0QA;
    delete reader;
    ++i;
  }
  auto c = new TCanvas();
  histPurityLambda->Draw();
  c->Print("PeriodQALambda.pdf");
  auto d = new TCanvas();
  histPurityAntiLambda->Draw();
  d->Print("PeriodQAAntiLambda.pdf");
}

void PeriodQA::ProcessSigmaQA(const char* prefix, const char* addon) {
  auto histPurityLambda = PeriodQAHist("histQALambda", "Purity #Lambda (%)");
  auto histPurityAntiLambda = PeriodQAHist("histQAAntiLambda",
                                           "Purity #bar{#Lambda} (%)");
  auto histPuritySigma = PeriodQAHist("histQASigma",
                                           "Purity #Sigma^{0} #oplus #bar{#Sigma^{0}} (%)");

  int i = 0;
  for (auto it : fPeriods) {
    TString filename = fDirectory.Data();
    filename += "AnalysisResults_";
    filename += it;
    filename += ".root";
    ForgivingReader* reader = new ForgivingReader(filename.Data(), prefix,
                                                  addon);
    DecayQA* v0QA = new DecayQA("#Lambda", "p#pi");
    v0QA->SetDecayCuts(reader->Getv0Cuts());
    v0QA->SetCanvasDivisions(5, 2);
    v0QA->SetIMHistoScale(1.75, 0.8, 0.35);
    v0QA->SetRangesFitting(1.109, 1.121, 1.09, 1.15);
    v0QA->GetPeriodQASigma(1.112, 1.120, it.Data());
    float purity = v0QA->GetPurity();
    float purityErr = v0QA->GetPurityErr();
    if (purity > 1E-6) {
      histPurityLambda->SetBinContent(i + 1, purity * 100.f);
      histPurityLambda->SetBinError(i + 1, purityErr * 100.f);
    }
    delete v0QA;

    DecayQA* antiv0QA = new DecayQA("#bar{#Lambda}", "p#pi");
    antiv0QA->SetDecayCuts(reader->GetAntiv0Cuts());
    antiv0QA->SetCanvasDivisions(5, 2);
    antiv0QA->SetIMHistoScale(1.75, 0.8, 0.35);
    antiv0QA->SetRangesFitting(1.109, 1.121, 1.09, 1.15);
    antiv0QA->GetPeriodQASigma(1.112, 1.120, it.Data());
    purity = antiv0QA->GetPurity();
    purityErr = antiv0QA->GetPurityErr();
    if (purity > 1E-6) {
      histPurityAntiLambda->SetBinContent(i + 1, purity * 100.f);
      histPurityAntiLambda->SetBinError(i + 1, purityErr * 100.f);
    }
    delete antiv0QA;

    DecayQA* sigma0QA = new DecayQA("#Sigma", "#Lambda#gamma");
        sigma0QA->SetDecayCuts(reader->GetOtherCuts("Sigma0Cuts"));
        sigma0QA->SetAntiDecayCuts(reader->GetOtherCuts("AntiSigma0Cuts"));
    sigma0QA->SetRangesFitting(1.182, 1.202, 1.167, 1.217);
    sigma0QA->GetPeriodQASigma0(0.003, it.Data());
    purity = sigma0QA->GetPurity();
    purityErr = sigma0QA->GetPurityErr();
    if (purity > 1E-6) {
      histPuritySigma->SetBinContent(i + 1, purity * 100.f);
      histPuritySigma->SetBinError(i + 1, purityErr * 100.f);
    }

    delete sigma0QA;
    delete reader;
    ++i;
  }
  auto c = new TCanvas();
  histPurityLambda->Draw("PE");
  c->Print("PeriodQALambda.pdf");
  auto d = new TCanvas();
  histPurityAntiLambda->Draw("PE");
  d->Print("PeriodQAAntiLambda.pdf");
  auto e = new TCanvas();
  histPuritySigma->Draw("PE");
  e->Print("PeriodQASigma.pdf");
}

TH1F* PeriodQA::PeriodQAHist(const char* name, const char* title) {
  auto histnew = new TH1F(name, Form("; ; %s", title), fPeriods.size(), 0,
                          fPeriods.size());
  histnew->SetMarkerStyle(20);
  int i = 1;
  for (auto it : fPeriods) {
    histnew->GetXaxis()->SetBinLabel(i++, it.Data());
  }
  histnew->LabelsOption("v", "X");
  return histnew;
}
