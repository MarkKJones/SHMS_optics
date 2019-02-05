#ifndef myConfig_h
#define myConfig_h 1

#include <string>
#include <vector>

#include "TMath.h"


namespace config {

  class BeamConfig {
    public:
      BeamConfig();
      ~BeamConfig();

      double x0;  // cm
      double y0;  // cm
      double xp0;
      double yp0;
  };

  class SHMSconfig {
    public:
      SHMSconfig();
      ~SHMSconfig();

      double thetaCentral;  // deg
      double cosTheta;
      double sinTheta;

      double thetaOffset;
      double phiOffset;

      double xMispointing;  // cm
      double yMispointing;  // cm
  };

 class SieveConfig {
    public:
      SieveConfig();
      ~SieveConfig();

      size_t nRow;
      size_t nCol;

      double xHoleMin;  // cm
      double yHoleMin;  // cm
      double xHoleSpace;  // cm
      double yHoleSpace;  // cm

      double x0;  // cm
      double y0;  // cm
      double z0;  // cm
  };

  class RunConfig {
  public:
    RunConfig();
    ~RunConfig();
    
    int runNumber;
    std::vector<std::string> fileList;
    std::string cuts;
    std::vector<double> zFoils;  // cm, zFoilOffset applied
    std::vector<double> Theta; //degrees
    
    BeamConfig beam;
    SHMSconfig SHMS;
    double sievetype; //1 for centered, 2 for offset
    SieveConfig sieve;
    std::vector<double> getSieveHolesX() const;
    std::vector<double> getSieveHolesY() const;
    int use2017Corr;

  };




  class Config{
    public:
      Config();
      ~Config();

      
      std::string recMatrixFileNameOld;
      std::string recMatrixFileNameNew;
      int fitOrder;
      int maxEventsPerHole;
      double zFoilOffset;  // cm

      int xTarCorrIterNum;

      std::vector<RunConfig> runConfigs;
  };


  std::vector<std::string> tokenize(const std::string& line);
  Config loadConfigFile(const std::string& fname);

}

#endif  // myConfig_h
