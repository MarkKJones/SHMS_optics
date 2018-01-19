#include "myEvent.hpp"

// ROOT includes.
#include "TChain.h"
#include "TTree.h"


// Implementation of Event.

Event::Event() :
  xFp(0.0), yFp(0.0), xpFp(0.0), ypFp(0.0),
  xVer(0.0), yVer(0.0), zVer(0.0),
  xTar(0.0), yTar(0.0), xpTar(0.0), ypTar(0.0),
  xTarVer(0.0), yTarVer(0.0), zTarVer(0.0),
  xSieve(0.0), ySieve(0.0)
{}


Event::~Event() {}


void Event::reset() {
  xFp = 0.0;
  yFp = 0.0;
  xpFp = 0.0;
  ypFp = 0.0;

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
  Double_t hsxfp, hsyfp, hsxpfp, hsypfp, frx_cm, fry_cm;

  TChain* chain = new TChain("T");
  for (const auto& fileName : runConf.fileList) {
    chain->Add(fileName.c_str());
  }

  TTree* tree = chain->CopyTree(runConf.cuts.c_str());

  tree->SetBranchAddress("P.dc.x_fp", &hsxfp);
  tree->SetBranchAddress("P.dc.y_fp", &hsyfp);
  tree->SetBranchAddress("P.dc.xp_fp", &hsxpfp);
  tree->SetBranchAddress("P.dc.yp_fp", &hsypfp);
  tree->SetBranchAddress("P.react.x", &frx_cm);
  tree->SetBranchAddress("P.react.y", &fry_cm);

  Long64_t nEntries = tree->GetEntries();
  //nEntries = 10000;  // TMP
  std::vector<Event> events(static_cast<std::size_t>(nEntries));
  std::vector<Event>::iterator it = events.begin();

  for (Long64_t iEntry=0; iEntry<nEntries; ++iEntry) {
    tree->GetEntry(iEntry);

    it->xFp = hsxfp;
    it->yFp = hsyfp;
    it->xpFp = hsxpfp;
    it->ypFp = hsypfp;

    it->xVer = -(runConf.beam.x0 - frx_cm);
    it->yVer = runConf.beam.y0 - fry_cm;

    ++it;
  }

  delete tree;
  delete chain;

  return events;
}
