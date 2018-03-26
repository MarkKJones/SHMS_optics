#include "myConfig.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// BeamConfig implementation.

config::BeamConfig::BeamConfig() : x0(0.0), y0(0.0), xp0(0.0), yp0(0.0) {}


config::BeamConfig::~BeamConfig() {}


// SHMSconfig implementation.

config::SHMSconfig::SHMSconfig() :
  thetaCentral(0.0), cosTheta(1.0), sinTheta(0.0),
  thetaOffset(0.0), phiOffset(0.0),
  xMispointing(0.0), yMispointing(0.0)
  //xMispointing(-0.126), yMispointing(-0.05)
{}


config::SHMSconfig::~SHMSconfig() {}

// SieveConfig implementation.

config::SieveConfig::SieveConfig() :
  nRow(11), nCol(11),
  xHoleMin(-12.5), yHoleMin(-8.2), xHoleSpace(2.5), yHoleSpace(1.64),
  x0(0.0), y0(0.0), z0(253.0)
{}

config::SieveConfig::~SieveConfig() {}

// RunConfig implementation.

config::RunConfig::RunConfig() :
  runNumber(0), fileList(), cuts(""), zFoils(),
  beam(), SHMS(), sievetype(0), sieve(), use2017Corr(0)
{}

config::RunConfig::~RunConfig() {}

std::vector<double> config::RunConfig::getSieveHolesX() const {
  std::vector<double> xSieveHoles(sieve.nRow);

  for (size_t i=0; i<sieve.nRow; ++i) {
    xSieveHoles.at(i) =
      sieve.xHoleMin + static_cast<double>(i)*sieve.xHoleSpace +
      sieve.x0;
  }

  return xSieveHoles;
}

std::vector<double> config::RunConfig::getSieveHolesY() const {
  std::vector<double> ySieveHoles(sieve.nCol);

  if (sievetype<2){//assume centered
    for (size_t i=0; i<sieve.nCol; ++i) {
      ySieveHoles.at(i) =
	sieve.yHoleMin + static_cast<double>(i)*sieve.yHoleSpace +
	sieve.y0;
    }
  }
  else{//shifted
    for (size_t i=0; i<sieve.nCol; ++i) {
      ySieveHoles.at(i) =
	sieve.yHoleMin + 0.82 + static_cast<double>(i)*sieve.yHoleSpace +
	sieve.y0;
    }
  }

  return ySieveHoles;
}

// Config implementation.

config::Config::Config() :
  recMatrixFileNameOld(""), recMatrixFileNameNew(""),
  fitOrder(0), maxEventsPerHole(0), zFoilOffset(0.0),
  xTarCorrIterNum(1), runConfigs()//, sieve()
{}


config::Config::~Config() {}




// Implementation of functions.

std::vector<std::string> config::tokenize(const std::string& line) {
  std::vector<std::string> tokens;
  std::string item;
  std::stringstream ss;
  ss.str(line);

  while(getline(ss, item, ' ')) {
    if (!item.empty()) tokens.push_back(item);
  }

  return tokens;
}


config::Config config::loadConfigFile(const std::string& fname) {
  Config conf;

  std::ifstream ifs(fname);
  std::string line;
  std::vector<std::string> tokens;

  if (!ifs.is_open()) {
    throw std::runtime_error("Could not open file: `"+fname+"`!");
  }

  while (getline(ifs, line)) {
    if (line.empty()) continue;

    tokens = tokenize(line);
    if (tokens[0][0] == '#') continue;
    if (tokens[0] == "endlist") break;

    if (tokens[0] == "newrun") {
      conf.runConfigs.push_back(RunConfig());
      conf.runConfigs.back().runNumber = stoi(tokens[1]);
    }
    else if (tokens[0] == "filelist") {
      conf.runConfigs.back().fileList = std::vector<std::string> (tokens.begin()+1, tokens.end());
    }
    else if (tokens[0] == "beampos") {
      conf.runConfigs.back().beam.x0 = stod(tokens[1]);
      conf.runConfigs.back().beam.y0 = stod(tokens[2]);
      conf.runConfigs.back().beam.xp0 = stod(tokens[3]);
      conf.runConfigs.back().beam.yp0 = stod(tokens[4]);
    }
    else if (tokens[0] == "thetaSHMS") {
      conf.runConfigs.back().SHMS.thetaCentral = stod(tokens[1]);
      conf.runConfigs.back().SHMS.cosTheta = cos(conf.runConfigs.back().SHMS.thetaCentral*TMath::DegToRad());
      conf.runConfigs.back().SHMS.sinTheta = sin(conf.runConfigs.back().SHMS.thetaCentral*TMath::DegToRad());
    }
    else if (tokens[0] == "zfoil") {
      for (size_t i=1; i<tokens.size(); ++i) {
        conf.runConfigs.back().zFoils.push_back(stod(tokens.at(i)));
      }
    }
    else if (tokens[0] == "sieveslit") {
      conf.runConfigs.back().sievetype = stod(tokens[1]);
      if (conf.runConfigs.back().sievetype>1){//shifted sieve
	conf.runConfigs.back().sieve.nCol = 10;
      }
    }
    else if (tokens[0] == "cut") {
      conf.runConfigs.back().cuts = tokens[1];
    }
    else if (tokens[0] == "mispointing") {
      conf.runConfigs.back().SHMS.xMispointing = stod(tokens[1]);
      conf.runConfigs.back().SHMS.yMispointing = stod(tokens[2]);
    }
    else if (tokens[0] == "use2017Corr"){
      //correction applied to 2017 data prior to optimization, corrects yTar ypTar dependence
      conf.runConfigs.back().use2017Corr = stod(tokens[1]);
    }
  }

  double thetaOffset;
  double phiOffset;
  double tmp;

  ifs >> conf.recMatrixFileNameOld;
  ifs >> conf.recMatrixFileNameNew;
  ifs >> thetaOffset >> phiOffset;
  ifs >> conf.fitOrder;
  ifs >> conf.maxEventsPerHole >> tmp;
  ifs >> conf.zFoilOffset;

  // Post-process.
  for (auto& runConf : conf.runConfigs) {
    runConf.SHMS.thetaOffset = thetaOffset;
    runConf.SHMS.phiOffset = phiOffset;

    for (auto& zFoil : runConf.zFoils) {
      zFoil += conf.zFoilOffset;
    }
  }

  return conf;
}
