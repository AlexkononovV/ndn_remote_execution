#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
//using namespace std;





extern "C" std::string function(std::string sens) {
  float res;
  int i=0;
  size_t pos = 0;
  std::string delimiter = ",";
  std::vector<std::string> vec;
  std::string token;
  while ((pos = sens.find(delimiter)) != std::string::npos) {
      token = sens.substr(0, pos);
      vec.push_back(token);
      sens.erase(0, pos + delimiter.length());
  }

  for (std::string s : vec){
    //float temp = std::stof(s);
    //float r3 = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
    std::srand (static_cast <unsigned> (time(0)));
    float r3 = 20.0 + static_cast <float> (std::rand()) /( static_cast <float> (RAND_MAX/(30.0-20.0)));
    res+=r3;
    i++;
  }
  std::cout << "I:" << i << std::endl;
  res=res/i;
  std::string reslt = std::to_string(res);
  //std::cout << ">> average....." << reslt << '\n';

  return reslt;
}


/*extern "C" std::string sum(std::string sens) {
  float res;
  int i=0;
  size_t pos = 0;
  std::string delimiter = ",";
  std::vector<std::string> vec;
  std::string token;
  while ((pos = sens.find(delimiter)) != std::string::npos) {
      token = sens.substr(0, pos);
      vec.push_back(token);
      sens.erase(0, pos + delimiter.length());
  }

  for (std::string s : vec){
    float temp = std::stof(s);
    res+=temp;
  }
  std::string reslt = std::to_string(res);
  std::cout << ">> sum....." << reslt << '\n';
  return reslt;
}*/