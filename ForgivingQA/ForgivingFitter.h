/*
 * ForgivingFitter.h
 *
 *  Created on: 13 Feb 2019
 *      Author: bernhardhohlweger
 */

#ifndef FORGIVINGQA_FORGIVINGFITTER_H_
#define FORGIVINGQA_FORGIVINGFITTER_H_
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TFitResultPtr.h"
class ForgivingFitter {
 public:
  ForgivingFitter();
  virtual ~ForgivingFitter();
//  void FitTheLambda(TH1F* histo, float &signal, float &signalErr,
//                    float &background, float &backgroundErr, float lowerBound,
//                    float upperBound, TCanvas *c1);
  void FitInvariantMass(TH1F* histo, float massCutMin, float massCutMax);
  void SetRanges(float SigMin, float SigMax, float BkgRangeMin,
                 float BkgRangeMax);
  int GetSignalCounts() {
    return fSignalCounts;
  }
  ;
  int GetBackgroundCounts() {
    return fBackgroundCounts;
  }
  ;
  float GetMeanMass() {
    return weightedMean(fWeightA,
                        fFullFitFnct->GetParameter(4),
                        fWeightB,
                        fFullFitFnct->GetParameter(7));
  }
  float GetMeanWidth() {
    return weightedMean(fWeightA,
                        fFullFitFnct->GetParameter(5),
                        fWeightB,
                        fFullFitFnct->GetParameter(8));
  }
 private:
  void CreateBackgroundFunction();
  void CreateContinousBackgroundFunction();
  void CreateSignalFunctions();
  void CreateFullFitFunction(TH1F* targetHisto);
  void SetStartParsDoubleGaussian(TH1F* targetHisto);
  float weightedMeanError(float weightA, float A, float weightB, float B,
                          float weightAErr, float AErr, float weightBErr,
                          float BErr);
  float weightedMean(float weightA, float A, float weightB, float B);
  TH1F *getSignalHisto(TF1 *function, TH1F *histo, float rangeLow,
                       float rangeHigh, const char *name);
  void CalculateBackgorund(TH1F* targetHisto, float massCutMin,
                           float massCutMax);
  TF1* fBackGround;
  TF1* fContinousBackGround;
  TF1* fSingleGaussian;
  TF1* fDoubleGaussian;
  TF1* fFullFitFnct;
  bool fRangesSet;
  float fBkgRangeMin;
  float fBkgRangeMax;
  float fSigRangeMin;
  float fSigRangeMax;
  int fSignalCounts;
  int fWeightA;
  int fWeightB;
  int fBackgroundCounts;
};

#endif /* FORGIVINGQA_FORGIVINGFITTER_H_ */