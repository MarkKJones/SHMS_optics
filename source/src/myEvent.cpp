#include "myEvent.hpp"

// ROOT includes.
#include "TChain.h"
#include "TTree.h"
#include <iostream>
#include "TFile.h"



// Implementation of Event.

Event::Event() :
  xFp(0.0), yFp(0.0), xpFp(0.0), ypFp(0.0),
  xVer(0.0), yVer(0.0), zVer(0.0),
  xTar(0.0), yTar(0.0), xpTar(0.0), ypTar(0.0),
  xTarVer(0.0), yTarVer(0.0), zTarVer(0.0),
  xSieve(0.0), ySieve(0.0), delta(0.0), theta(0.0)
{}


Event::~Event() {}


void Event::reset() {
  xFp = 0.0;
  yFp = 0.0;
  xpFp = 0.0;
  ypFp = 0.0;
  delta = 0.0;
  theta = 0.0;

  xVer = 0.0;
  yVer = 0.0;
  zVer = 0.0;

  xTar = 0.0;
  yTar = 0.0;
  xpTar = 0.0;
  ypTar = 0.0;

  xTarVer = 0.0;
  yTarVer = 0.0;
  zTarVer = 0.0;

  xSieve = 0.0;
  ySieve = 0.0;
}


// Implementation of other functions.

std::vector<Event> readEvents(const config::RunConfig& runConf) {
  Double_t hsxfp, hsyfp, hsxpfp, hsypfp, frx_cm, fry_cm, dp;

  Long64_t entriesTotal = 0.0;

  //std::cout<<"entries: "<<entriesTotal<<std::endl;

  for (const auto& fileName : runConf.fileList) {
    //std::cout<<"Filename: "<<fileName<<std::endl;

    TFile *f = new TFile(fileName.c_str());
    TTree *tree = (TTree*)f->Get("T");
    Long64_t nEntries = tree->GetEntries();
    entriesTotal += nEntries;
  }

  std::vector<Event> events(static_cast<std::size_t>(entriesTotal));
  std::vector<Event>::iterator it = events.begin();
  int iList = 0;
  for (const auto& fileName : runConf.fileList) {

    double iTheta = runConf.Theta.at(iList); 
    TFile *f = new TFile(fileName.c_str());
    TTree *tree = (TTree*)f->Get("T");

    tree->SetBranchAddress("P.dc.x_fp", &hsxfp);
    tree->SetBranchAddress("P.dc.y_fp", &hsyfp);
    tree->SetBranchAddress("P.dc.xp_fp", &hsxpfp);
    tree->SetBranchAddress("P.dc.yp_fp", &hsypfp);
    tree->SetBranchAddress("P.react.x", &frx_cm);
    tree->SetBranchAddress("P.react.y", &fry_cm);
    tree->SetBranchAddress("P.gtr.dp", &dp);
    
   
    Long64_t nEntries = tree->GetEntries();
    //nEntries = 10000;  // TMP    
    for (Long64_t iEntry=0; iEntry<nEntries; ++iEntry) {
      tree->GetEntry(iEntry);
      
      it->xFp = hsxfp;
      it->yFp = hsyfp ;//+ 0.613;
      it->xpFp = hsxpfp;
      it->ypFp = hsypfp;
      it->delta = dp;

      it->xVer = frx_cm;
      it->yVer = fry_cm;
      it->theta = iTheta;

      ++it;
    }//end entries
    f->Close();
    iList++;
  }//end file loop

  return events;
}
