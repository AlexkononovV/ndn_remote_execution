/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2022 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/validator-config.hpp>
#include "ndn-cxx/encoding/tlv.hpp"
#include "ndn-cxx/encoding/block.hpp"
#include <iostream>

#include <cstddef>
#include <bitset>
#include <ndn-cxx/encoding/buffer.hpp>

#include "producer_consumer.hpp"
#include <boost/regex.hpp>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <map>

#include <functional>
#include <algorithm>
#include <thread>
#include <fstream>


// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces should be used to prevent/limit name conflicts
namespace examples {

//class Consumer{
//public:

  Consumer::Consumer()
  //:m_face("192.168.177.10","6363")
  {
    //m_validator.load("trust-schema_default.conf");
    //prefix = f_prefix;
  }

  //***********************************************************
  //#      1. Register remote function prefix                 #
  //***********************************************************
  std::string
  Consumer::functionPrefix(std::string pref)
  {

    auto tme = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(tme);
    auto time = std::ctime(&end_time);
    std::hash<char*> ptr_hash;
    auto aux = (ptr_hash(time));

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string id = boost::lexical_cast<std::string>(uuid);

    id = id + std::to_string(aux); 

    results[id] = ""; //resultado da execução 
    prefixes[id] = pref; //prefixo da função para ser executada
    consumer_prefix[id]="/consumer/install/1";
    return id;
  }
  //-----------------------------------------------------------

  //***********************************************************
  //#      2. Set Input Arguments                             #
  //***********************************************************
  void Consumer::setArguments(std::string id, std::string arguments, bool mode, int length){ 
    int size = arguments.length();
    data_input[id] = arguments;
    data_length[id]=length;
    modes[id] =  mode;
    std::cout << "-->mode set: "<<modes[id]<< std::endl;
    std::cout << "-->size of args : " << size << std::endl;
    if(size < 100){ 
      type_input[id]=1;
    }
    else{
      type_input[id]=0;
    }
  }
  //-----------------------------------------------------------

  //***********************************************************
  //#      3. Execute                                         #
  //***********************************************************
  void
  Consumer::execute(std::string id, bool mode){
    TM_beforexecute = std::chrono::steady_clock::now();;


    if (type_input[id]){
      execute_explicit(id);
      //auto TM_afterexecute = std::chrono::steady_clock::now();
      //auto exec_time = std::chrono::duration_cast<std::chrono::milliseconds>(TM_afterexecute - TM_beforexecute).count();
      //std::cout << "TIME elapsed from start of execution in ms: " << exec_time << std::endl;
      //std::ofstream outfile;
      /*if(mode){
        outfile.open("tests/total_exec_N_ex.txt", std::ios_base::app); // append instead of overwrite
      }
      else{
        outfile.open("tests/total_exec_P_ex.txt", std::ios_base::app);
      }
      
      outfile << exec_time;
      outfile << "\n";*/
    }
    else{
      execute_implicit(id);
      //auto TM_afterexecute = std::chrono::steady_clock::now();
      //auto exec_time = std::chrono::duration_cast<std::chrono::milliseconds>(TM_afterexecute - TM_beforexecute).count();
      //std::cout << "TIME elapsed from start of execution in ms: " << exec_time << std::endl;
      //std::ofstream outfile;
      /*if(mode){
        outfile.open("tests/total_exec_N_im.txt", std::ios_base::app); // append instead of overwrite
      }
      else{
        outfile.open("tests/total_exec_P_im.txt", std::ios_base::app);
      }
      
      outfile << exec_time;
      outfile << "\n";*/
    }
  }
  //-----------------------------------------------------------

  //***********************************************************
  //#      3a. Execute explicit                               #
  //***********************************************************
  void
  Consumer::execute_explicit(std::string id){
    std::cout << ">>Explicit arguments." << std::endl;
    //const char* input_bytes = data_input[id];
    //int size = sizeof input_bytes * data_length[id];
    
    //auto buf1 = Buffer(input_bytes, size);
    auto buf1 = Buffer(data_input[id].data(), data_input[id].size());
    auto buf_p1 = std::make_shared<Buffer>(buf1);
    Block buffer((uint32_t)400, buf_p1);
    buffer.encode();

    int length = data_length[id];
    const char* length_bytes = reinterpret_cast<const char*>(&length);
    auto buf2 = Buffer(length_bytes, 4); 
    auto buf_p2 = std::make_shared<Buffer>(buf2);
    Block length_block((uint32_t)401, buf_p2);
    length_block.encode();

    Block data((uint32_t)300);
    data.insert(data.elements_end() ,buffer);
    data.insert(data.elements_end() ,length_block);
    data.encode();
    
    const char* bytes = reinterpret_cast<const char*>(&modes[id]);
    auto buf5 = Buffer(bytes, sizeof(bool) ); //sizeof modes[id]); 
    auto buf_p5 = std::make_shared<Buffer>(buf5);
    Block mode_block((uint32_t)404, buf_p5);
    mode_block.encode();

    auto buf3 = Buffer(id.data(), id.size()); 
    auto buf_p3 = std::make_shared<Buffer>(buf3);
    Block id_block((uint32_t)402, buf_p3);
    id_block.encode();

    int type=2000;
    Block appParam((uint32_t)type);

    appParam.insert(appParam.elements_end() ,data);
    appParam.insert(appParam.elements_end() ,mode_block);
    appParam.insert(appParam.elements_end() ,id_block);

    if(modes[id]){
      std::cout << ">>Modo NOTIFY." << std::endl;
      
      run_prefix(id);

      auto buf4 = Buffer(consumer_prefix[id].data(), consumer_prefix[id].size()); 
      auto buf_p4 = std::make_shared<Buffer>(buf4);
      Block prefix_block((uint32_t)403, buf_p4);
      prefix_block.encode();

      appParam.insert(appParam.elements_end() ,prefix_block);
    }
    else{
      std::cout << ">>Modo POLLING." << std::endl;
    }

    appParam.encode();  

    sendInterest(prefixes[id], appParam);
    m_face.processEvents();
  }
  //-----------------------------------------------------------

  //***********************************************************
  //#      3b. Execute implicit                               #
  //***********************************************************
  void
  Consumer::execute_implicit(std::string id){
    std::cout << ">>Implicit arguments." << std::endl;
    run_prefix(id);

    auto buf3 = Buffer(id.data(), id.size()); 
    auto buf_p3 = std::make_shared<Buffer>(buf3);
    Block id_block((uint32_t)402, buf_p3);
    id_block.encode();

    auto buf4 = Buffer(consumer_prefix[id].data(), consumer_prefix[id].size()); 
    auto buf_p4 = std::make_shared<Buffer>(buf4);
    Block prefix_block((uint32_t)403, buf_p4);
    prefix_block.encode();

    const char* bytes = reinterpret_cast<const char*>(&modes[id]);
    auto buf5 = Buffer(bytes, sizeof(bool));//sizeof modes[id]); 
    auto buf_p5 = std::make_shared<Buffer>(buf5);
    Block mode_block((uint32_t)404, buf_p5);
    mode_block.encode();

    int type=2001;
    Block appParam((uint32_t)type);
    
    appParam.insert(appParam.elements_end() ,id_block);
    appParam.insert(appParam.elements_end() ,prefix_block);
    appParam.insert(appParam.elements_end() ,mode_block);
    appParam.encode();   

    sendInterest(prefixes[id], appParam);
    m_face.processEvents();
  }
  //-----------------------------------------------------------


  //***********************************************************
  //#      4. Register prefix                                 #
  //***********************************************************
  void
  Consumer::run_prefix(std::string id)
  {
    
    //m_currentSeqNo=0;
    m_prefixId = m_face.setInterestFilter(consumer_prefix[id],
                             std::bind(&Consumer::onInterest, this, _1, _2),
                             //RegisterPrefixSuccessCallback(),
                             std::bind(&Consumer::onPrefixRegistered, this, _1),
                             std::bind(&Consumer::onRegisterFailed, this, _1, _2));

    m_face.processEvents(ndn::time::milliseconds(100),false);
  }
  //-----------------------------------------------------------


  //***********************************************************
  //#      Aux. Send Interest                                 #
  //***********************************************************
  void
  Consumer::sendInterest(std::string prefix, Block appParams){

    std::cout << ">> Sending interest with type " << appParams.type() << " to prefix: "<< prefix<< std::endl;
    Name interestName(prefix);
    interestName.appendVersion();
    Interest interest(interestName);

    interest.setApplicationParameters( appParams );
    interest.setMustBeFresh(true);
    interest.setInterestLifetime(5_s); // The default is 4 seconds

    //std::cout << ">> Sending Interest " << interest << std::endl;
    m_face.expressInterest(interest,
                           std::bind(&Consumer::onData, this,  _1, _2),
                           std::bind(&Consumer::onNack, this, _1, _2),
                           std::bind(&Consumer::onTimeout, this, _1));

    // processEvents will block until the requested data is received or a timeout occurs
    
  }
  //-----------------------------------------------------------

  //***********************************************************
  //#      Aux. Send Data                                     #
  //***********************************************************
  void
  Consumer::sendData(const Name& prefix, Block content){
    auto data = make_shared<Data>(prefix);
    data->setFreshnessPeriod(10_s);
    data->setContent( content);//make_span(reinterpret_cast<const uint8_t*>(content.data()), content.size()));

    m_keyChain.sign(*data);
    // m_keyChain.sign(*data, signingByIdentity(<identityName>));
    // m_keyChain.sign(*data, signingByKey(<keyName>));
    // m_keyChain.sign(*data, signingByCertificate(<certName>));
    // m_keyChain.sign(*data, signingWithSha256());

    // Return Data packet to the requester
    std::cout << "<< Sending Data: " << content.type() << " : " << content << std::endl;
    m_face.put(*data);
  }

  //***********************************************************
  //#      Aux. On Data                                       #
  //***********************************************************
  void
  Consumer::onData(const Interest&, const Data& data) // std::string id)
  {
    //std::cout << "Received Data " << data << std::endl;
    /*
    m_validator.validate(data,
                         [] (const Data&) {
                           std::cout << "Data conforms to trust schema" << std::endl;
                         },
                         [] (const Data&, const security::ValidationError& error) {
                           std::cout << "Error authenticating data: " << error << std::endl;
                         });
    */
    /*std::string resp = std::string(reinterpret_cast<const char*>(data.getContent().value()),data.getContent().value_size());

    */
    Block response = data.getContent();
    response.parse();
    Block wrapper = response.elements()[0];
    wrapper.parse();
    int type = wrapper.type();//response.type();//
    std::string id_prod ="";
    std::string id ="";
    
    std::cout << "<< Data received, type: " << type << std::endl;
    switch(type){
        case 2100:{
          for ( auto e: wrapper.elements() )
          { 
            std::cout << "  << element inside wrapper: " << e << std::endl;
            if (e.type() == 498){
              auto c = wrapper.get(498);
              std::string ack =std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              //results[id] = s;
              std::cout << "  " << ack << std::endl;
            }
            if (e.type() == 405){
              auto c = wrapper.get(405);
              //producer_id[id] = std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              id_prod = std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
            }
            if (e.type() == 402){
              auto c = wrapper.get(402);
              //producer_id[id] = std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              id = std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              
            }
            if(e.type() == 499){
              auto c = wrapper.get(499);
              std::string err =std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              std::cout<<"  ERR: " << err << std::endl;
              exit(-1);
            }
          }
          
          if(!modes[id]){
            if (id_prod == ""){
              std::cout << "ERR: Producer id process not provided." << std::endl;
            }
            else{
              if(id == ""){
                std::cout << "ERR: Consumer id(402) not specified." << std::endl;
              }
              else{
                producer_id[id_prod] = id;
                fetchResult(id_prod);
              }
            }
              
          }
          

          break;
        }

        case 2101:
        {
          for ( auto e: wrapper.elements() )
          {
            if (e.type() == 498){
              auto in = wrapper.get(498);
              std::string ack  =std::string(reinterpret_cast<const char*>(in.value()), in.value_size());
              std::cout << ">>   " << ack << std::endl;
            }
            if( e.type() == 499){
              auto c = wrapper.get(499);
              std::string err =std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              std::cout<<"ERR: " << err << std::endl;
              exit(-1);
            }
          }
          
          break;
        }


        case 2102:
        case 2103:
        { std::string prod_id = "";
          std::string res = "";
          
          for ( auto e: wrapper.elements() )
          { 
            std::cout << "  << element inside wrapper: " << e << std::endl;
            if (e.type() == 301){
              auto in = wrapper.get(301);
              res = std::string(reinterpret_cast<const char*>(in.value()), in.value_size());
              //res =std::string(reinterpret_cast<const char*>(in.value()), in.value_size());
              //results[id] = res;

            }
            if (e.type() == 405){
              auto in = wrapper.get(405);
              prod_id =std::string(reinterpret_cast<const char*>(in.value()), in.value_size());
              //results[id] = res;
            }
            if( e.type() == 499){
              auto c = wrapper.get(499);
              std::string err =std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              std::cout<<"  >> ERR: " << err << std::endl;
              exit(1);
            }
          }
          if( prod_id != ""){
            if(res != ""){
              if (producer_id.find(prod_id) == producer_id.end()){
                std::cout << ">> ERR: Producer id (405) doesnt exist." << std::endl;
              }
              else{
                std::string id = producer_id[prod_id];
                
                results[id] = res;
                TM_afterexecute = std::chrono::steady_clock::now();
                auto completion_time = std::chrono::duration_cast<std::chrono::milliseconds>(TM_afterexecute - TM_beforexecute).count();
                std::ofstream outfile2;
                //std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||";
                outfile2.open("tests/TOTAL_C.txt", std::ios_base::app); // append instead of overwrite
                outfile2 << completion_time;
                outfile2 << "\n";

                m_face.shutdown();
              }
            } 
            else{
              if(!modes[id]){
                fetchResult(prod_id);
              }
              else{
                std::cout << ">> No result available." << std::endl;
              }
            }

          }
          else{
            std::cout << ">> ERR: No id(405) provided." << std::endl;
          }
          break;
        }


        
        default:{
         std::cout << ">>ERR: Unknown type " << type << std::endl;
         break;
       }
    }

  }

  //-----------------------------------------------------------

  //***********************************************************
  //#      Aux. On Interest to prefix                         #
  //***********************************************************
  void
  Consumer::onInterest(const InterestFilter& filter, const Interest& interest){
      
    if( interest.hasApplicationParameters() ) {

      std::cout << ">> Interest[app params]: " ;

      Block inter = interest.getApplicationParameters();
      inter.parse();
      Block wrapper = inter.elements()[0];
      wrapper.parse();
      int type = wrapper.type();

      std::cout << type << std::endl;

      std::string id = "";
      std::string id_prod = "";

      switch(type){
        case 2201:{
          for( auto e : wrapper.elements())
          {
            std::cout << "  << element inside wrapper: " << e << std::endl;
            if(e.type() == 402){
              Block block_id = wrapper.get(402);
              id =std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size());
            }
            if(e.type() == 405){
              Block block_id = wrapper.get(405);
              id_prod =std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size());
            }
          }
          if(id != ""){
            if(id_prod != ""){
              producer_id[id_prod] = id;
              std::string s = "ACK";
              auto buf = Buffer(s.data(), s.size()); 
              auto buf_p = std::make_shared<Buffer>(buf);
              Block ack_block((uint32_t)498, buf_p);
              ack_block.encode();

              Block wrapperout((uint32_t)2301);
              wrapperout.insert(wrapperout.elements_end() ,ack_block);
              wrapperout.encode();

              sendData(interest.getName(),wrapperout);
              //m_face.processEvents(ndn::time::milliseconds(500),false);

              fetchResult(id_prod);
            }
          }
          else{
            std::cout << "ERR: No id provided to fecth results." << std::endl;
          }

          break;
        }


        case 2200:{
          for( auto e : wrapper.elements()){
            std::cout << "  << element inside wrapper: " << e << std::endl;
          
            if(e.type() == 402){
              Block block_id = wrapper.get(402);
              id =std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size());
            }
            if(e.type() == 405){
              Block block_id = wrapper.get(405);
              id_prod =std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size());
              //std::cout << "id_prod = " << id_prod << std::endl;
            }
          }

          if( id == ""){
            std::cout << "No id(402)." << std::endl;
          }
          else{
            if (id_prod != ""){

              producer_id[id_prod] = id;

              //const char* aux = data_input[id];
              //int size = sizeof aux * data_length[id];
              //auto a_buf = Buffer(aux, size);
              auto a_buf = Buffer(data_input[id].data(), data_input[id].size());
              auto b_buf = std::make_shared<Buffer>(a_buf);
              Block buffer((uint32_t)400, b_buf);
              buffer.encode();

              int length = data_length[id];
              const char* bytes = reinterpret_cast<const char*>(&length);
              auto a2_buf = Buffer(bytes, 4); 
              auto b2_buf = std::make_shared<Buffer>(a2_buf);
              Block length_block((uint32_t)401, b2_buf);
              length_block.encode();

              Block data_block((uint32_t)300);
              data_block.insert(data_block.elements_end() ,buffer);
              data_block.insert(data_block.elements_end() ,length_block);
              data_block.encode();

              Block prodid = wrapper.get(405);

              Block wrapperout((uint32_t)2300);
              wrapperout.insert(wrapperout.elements_end() ,data_block);
              wrapperout.insert(wrapperout.elements_end() ,prodid);
              wrapperout.encode();

              sendData(interest.getName(),wrapperout);

              if(!modes[id]){
                fetchResult(id_prod);
              }


            }
            else{
              std::cout << "No id(405)." << std::endl;
            }
          }

          break;
        }
        default:
          std::cout << "Unknown type" << std::endl;
          break;
      }
    }
  }


  //-----------------------------------------------------------

  //***********************************************************
  //#      5. fetch Results from Producer                     #
  //***********************************************************
  void
  Consumer::fetchResult(std::string id405){
    
    //std::string id405 = producer_id[id];
    std::string id = producer_id[id405];
    std::cout << ">> Fetching result from producer. Prefix: " << prefixes[id] << " - ";
    

    auto b = Buffer(id405.data(),id405.size());
    auto p = std::make_shared<Buffer>(b);

    Block id_block((uint32_t)405,p);
    id_block.encode();  
    if(modes[id]){
      std::cout << " NOTIFY" << std::endl;
      Block wrapper((uint32_t)2002);
      wrapper.insert(wrapper.elements_end() ,id_block);
      wrapper.encode();
      sendInterest(prefixes[id], wrapper);
      
    }
    else{
      std::cout << " POLLING" << std::endl;
      //sleep(5000);
      //sleep_until(system_clock::now() + seconds(1));
      //std::this_thread::sleep_for(std::chrono::nanoseconds(1000000000));
      std::this_thread::sleep_for(std::chrono::microseconds(2037000));;
      Block wrapper((uint32_t)2003);
      wrapper.insert(wrapper.elements_end() ,id_block);
      wrapper.encode();

      sendInterest(prefixes[id], wrapper);
      //m_face.processEvents();
    } 

  }
  //-----------------------------------------------------------

  //***********************************************************
  //#      6. app polling results                             #
  //***********************************************************
  std::string
  Consumer::getResponse(std::string id){
    
    return results[id];
  }
  //-----------------------------------------------------------


  //***********************************************************
  //#      Aux. NDN                                           #
  //***********************************************************
  
  void Consumer::onPrefixRegistered(const Name& prefix) {

    std::cout << ">> Prefix " << prefix << " registered." << std::endl;
  }

  void
  Consumer::onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix '" << prefix
              << "' with the local forwarder (" << reason << ")" << std::endl;
    m_face.shutdown();
  }

  void
  Consumer::onNack(const Interest&, const lp::Nack& nack) const
  {
    std::cout << "Received Nack with reason " << nack.getReason() << std::endl;
  }

  void
  Consumer::onTimeout(const Interest& interest) const
  {
    std::cout << "Timeout for " << interest << std::endl;
  }

  //-----------------------------------------------------------


} // namespace examples
} // namespace ndn

