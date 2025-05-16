#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct CheckPoint {
  std::string name;
  float latitude;
  float longitude;
  float penalty;
  bool required;

  CheckPoint(std::string name, float latitude, float longitude)
      : name(name), latitude(latitude), longitude(longitude), penalty(0),
        required(true) {};

  CheckPoint(std::string name, float latitude, float longitude, float penalty)
      : name(name), latitude(latitude), longitude(longitude), penalty(penalty),
        required(false) {};
};

class ReportBuilder {
public:
  virtual void addCheckpoint(const CheckPoint &CheckPoint) = 0;
  virtual void addCheckpoints(const std::vector<CheckPoint> &route) {
    for (auto cp : route) {
      addCheckpoint(cp);
    }
  };

  virtual std::string GetReport() = 0;
};

class PrintReportBuilder : public ReportBuilder {
public:
  void addCheckpoint(const CheckPoint &cp) override {
    std::ostringstream ss;
    ss << "CheckPoint " << cp.name << ": (" << cp.latitude << ", "
       << cp.longitude << "), penalty: ";
    if (cp.required) {
      ss << "failure of SU\n";
    } else {
      ss << cp.penalty << "\n";
    }
    report += ss.str();
  }

  std::string GetReport() override { return report; }

private:
  std::string report = "";
};

class PenaltyReportBuilder : public ReportBuilder {
public:
  void addCheckpoint(const CheckPoint &cp) override {
    if (!cp.required) {
      penalty += cp.penalty;
    }
  }

  std::string GetReport() override { return std::to_string(penalty); }

private:
  float penalty = 0;
};

int main() {
  std::vector<CheckPoint> route{CheckPoint("Start", 34.232, 44.543),
                                CheckPoint("Third", 1.123, 45.124, 100),
                                CheckPoint("Two thirds", 54.786, 24.133, 300),
                                CheckPoint("Finish", 20.8, 4.3)};

  ReportBuilder *printBuilder = new PrintReportBuilder();
  printBuilder->addCheckpoints(route);
  std::cout << "Print Report:\n" << printBuilder->GetReport() << std::endl;

  ReportBuilder *penaltyBuilder = new PenaltyReportBuilder();
  penaltyBuilder->addCheckpoints(route);
  std::cout << "Penalty Report:\n" << penaltyBuilder->GetReport() << std::endl;

  return 0;
}
