#ifndef MODEL_H
#define MODEL_H

#include "Component.h"
#include <string>

class Model : public Component {
 private:
  char filePath[100];

 public:
  void setPath(std::string path);

  std::string getPath() { return std::string(filePath); }

 protected:
  void serializeComponent(std::ofstream* fileNamePathContentObject) override;
  void deserializeComponent(std::ifstream* fileNamePathContentObject) override;

 public:
  void saveComponent(std::ofstream* fileNamePathContentObject) override;
  void loadComponent(std::ifstream* fileNamePathContentObject) override;
};
#endif