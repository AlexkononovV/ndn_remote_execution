#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include <iostream>



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

  void
  run()
  {
    //const char* host = "127.0.0.1";
    //const char* port = "6363";
    //const unsigned short port = 6363;
    //const char* host = "0.0.0.0";
    //m_face( (const std::string&) host, (const std::string&) port);

    std::cout << "Running...."<< std::endl;
    Name interestName("/iot/temp/1");
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
  }

private:
  void
  onData(const Interest&, const Data& data)
  {
    std::cout << "Received Data " << data << std::endl;

    std::cout << std::string(reinterpret_cast<const char*>(data.getContent().value()),
                                                           data.getContent().value_size());
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
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{ 
  try {
    ndn::examples::Consumer consumer;
    consumer.run();
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
}
