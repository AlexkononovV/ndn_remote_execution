#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>


#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/encoding/block.hpp"

//using namespace std;



// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces should be used to prevent/limit name conflicts
namespace examples {

class Consumer
{
public:

  Consumer()
  :m_face("192.168.177.10","6363")
  {
    
  }

  std::string
  run(std::string pref)
  {
    //const char* host = "127.0.0.1";
    //const char* port = "6363";
    //const unsigned short port = 6363;
    //const char* host = "0.0.0.0";
    //m_face( (const std::string&) host, (const std::string&) port);

    std::cout << "Running...."<< std::endl;
    Name interestName(pref);
    interestName.appendVersion();

    Interest interest(interestName);

    interest.setMustBeFresh(true);
    interest.setInterestLifetime(6_s); // The default is 4 seconds

    std::cout << "Sending Interest " << interest << std::endl;
    m_face.expressInterest(interest,
                           std::bind(&Consumer::onData, this,  _1, _2),
                           std::bind(&Consumer::onNack, this, _1, _2),
                           std::bind(&Consumer::onTimeout, this, _1));

    // processEvents will block until the requested data is received or a timeout occurs
    m_face.processEvents();
    return res;
  }

private:
  void
  onData(const Interest&, const Data& data)
  {
    std::cout << "Received Data " << data << std::endl;
    res= std::string(reinterpret_cast<const char*>(data.getContent().value()),
                                                           data.getContent().value_size());
    std::cout << res;
  }

  void
  onNack(const Interest&, const lp::Nack& nack) const
  {
    std::cout << "Received Nack with reason " << nack.getReason() << std::endl;
  }

  void
  onTimeout(const Interest& interest) const
  {
    std::cout << "Timeout for " << interest << std::endl;
  }

private:
  Face m_face;
  ValidatorConfig m_validator{m_face};
  std::string res;
};

} // namespace examples
} // namespace ndn



extern "C" std::string function(std::string sens) {
  size_t pos = 0;
  std::string delimiter = ",";
  std::vector<std::string> vec;
  std::vector<std::string> datas;
  std::string token;
  while ((pos = sens.find(delimiter)) != std::string::npos) {
      token = sens.substr(0, pos);
      vec.push_back(token);
      sens.erase(0, pos + delimiter.length());
  }

  for (std::string s : vec){
    ndn::examples::Consumer consumer;
    std::string reslt =consumer.run(s);
    datas.push_back(reslt);
  }

  /*float res;
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
  res=res/i;
  std::string reslt = std::to_string(res);*/

  return datas[0];
}
