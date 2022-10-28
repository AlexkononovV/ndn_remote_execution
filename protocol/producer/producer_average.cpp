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
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

#include "boost/lexical_cast.hpp"

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <map>

#include <functional>
#include <algorithm>
#include <chrono>

#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */

#include <dlfcn.h>
#include <thread>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespaces should be used to prevent/limit name conflicts
namespace examples {

class Producer
{
public:

  Producer()
  :m_face("127.0.0.1","6363")
  { 
    /*config_file.open("configs.txt", std::ios::in);
    if (!config_file) {
      std::cout << "No config file";
    }
    else{
      std::string myline;
      while ( config_file ) {
        std::getline (config_file, myline);
        std::string delimiter = ":";
        std::string code = myline.substr(0, myline.find(delimiter));
        if(code=="Port"){
          port_serv=stoi(myline.erase(0, myline.find(delimiter) + delimiter.length()));
          std::cout << "---PORT: " << port_serv << std::endl;
        }
        if(code=="Host"){
          host_serv=myline.erase(0, myline.find(delimiter) + delimiter.length());
          std::cout << "---HOST: " << host_serv << std::endl;
        }
        
        
        //std::cout << myline << //": " << config_file.tellg() << '\n';
        }
    }*/
    
  }

  //***********************************************************
  //#     RUN                                                 #
  //***********************************************************
  void
  run()
  { auto one = std::chrono::steady_clock::now();
    m_face.setInterestFilter("producer/computation/average" , //"/protocol/computation/",
                             std::bind(&Producer::onInterest, this, _1, _2),
                             nullptr, // RegisterPrefixSuccessCallback is optional
                             std::bind(&Producer::onRegisterFailed, this, _1, _2));

    /*auto cert = m_keyChain.getPib().getDefaultIdentity().getDefaultKey().getDefaultCertificate();
    m_certServeHandle = m_face.setInterestFilter(security::extractIdentityFromCertName(cert.getName()),
                             [this, cert] (auto&&...) {
                               m_face.put(cert);
                             },
                             std::bind(&Producer::onRegisterFailed, this, _1, _2));
    */
    auto two = std::chrono::steady_clock::now();
    auto regtime = std::chrono::duration_cast<std::chrono::milliseconds>(two - one).count();
    std::cout << "PREFIX REG TIME :" << regtime << "\n";
    std::ofstream outfile;
    outfile.open("time.txt", std::ios_base::app); // append instead of overwrite
    outfile << regtime;
    m_face.processEvents();
  }

private:

  //***********************************************************
  //#     OnInterest                                          #
  //***********************************************************
  void
  onInterest(const InterestFilter&, const Interest& interest)
  {     
    if( interest.hasApplicationParameters()) {

      Block appParams = interest.getApplicationParameters();
      appParams.parse();
   
      Block wrapper = appParams.elements()[0];
      wrapper.parse();
      int type = wrapper.type();

      //const unsigned char* buf_arg;
      std::string buf_arg;
      int args_length;
      bool mode;
      std::string consumer_prefix = "";
      std::string id="";
      std::cout << ">> Interest type " << type << std::endl;

      switch(type){
        
        case 2000:{
          for ( auto e: wrapper.elements() )
          { 
            std::cout << "  << element inside wrapper: " << e << std::endl;
            if (e.type() == 300)
            {
              
              Block input = wrapper.get(300);
              input.parse();

              for(auto e2 : input.elements() )
              {
                std::cout << "  << element inside input: " << e2 << std::endl;
                if(e2.type() == 400 ){
                  Block buffer = input.get(400);
                  // buffer.parse();

                  //buf_arg = reinterpret_cast<const unsigned char*>(buffer.value());
                  buf_arg=std::string(reinterpret_cast<const char*>(buffer.value()), buffer.value_size());
                }
                if(e2.type() == 401 ){
                  Block length = input.get(401);
                  //length.parse();
                  const char* aux = reinterpret_cast<const char*>(length.value());
                  args_length = *reinterpret_cast<const int*>(aux);
                  //auto v = std::vector< unsigned char>(aux, aux + length.value_size());
                  
                  //for(auto a : v) {std::cout << (int)a << std::endl;}
                  //std::string value(v.begin(), v.end());
                  //std::cout << "value: " << value << std::endl;
                  //args_length = stoi(value);
                  
                }
                else{
                  std::cout << "  unknown type" << std::endl;
                }
              }
            }
            //________
            if (e.type() == 402){
              Block block_id = wrapper.get(402);
              //id_block.parse();
              id = std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size()); 
            }

            if(e.type() == 403){
              Block prefix = wrapper.get(403);
              //prefix.parse();
              consumer_prefix = std::string(reinterpret_cast<const char*>(prefix.value()), prefix.value_size()); 
              std::cout << "    << Consumer prefix: " << consumer_prefix << std::endl;
            }
            if(e.type() == 404){
              Block mode_op = wrapper.get(404);
              //mode_op.parse();
              const char* aux = reinterpret_cast<const char*>(mode_op.value());
              mode = *reinterpret_cast<const bool*>(aux); //(bool)aux;//reinterpret_cast<char*>(aux);
              std::cout << "    << Operation mode: " << mode << std::endl;
            }
            if(e.type() == 499){
              Block err = wrapper.get(499);
              //err.parse();
              std::cout << "ERROR: " << std::string(reinterpret_cast<const char*>(err.value()), err.value_size())  << std::endl;
            }
          }
          // end of for
          Block wrapperout((uint32_t)2100);
          std::string id405 = generate_id();
          ids[id405] = id;
          modes[id405] = mode;

          
          /*std::string prfx = interest.getName().toUri();
          size_t pos = 0;
          std::string delimiter = "/";
          std::vector<std::string> vec;
          std::string token;
          while ((pos = prfx.find(delimiter)) != std::string::npos) 
          {
                token = prfx.substr(0, pos);
                vec.push_back(token);
                prfx.erase(0, pos + delimiter.length());
          }
          int in=0;
          std::string plugin="";
          for(std::string token : vec){
              if(token == "computation"){
                  plugin=vec[in+1];
                  break;
              }
              in++;
          }*/
          std::string plugin="average";
          std::cout << ">> Plugin: " << plugin << std::endl;
          plugins[id405]=plugin;

          if(id == ""){
            std::cout << "ERR: no consumer id (402) provided." << std::endl;
            //TODO send Block 499
          }
          else{

            std::string ack = "ACK";
            auto buf = Buffer(ack.data(), ack.size()); 
            auto buf_p = std::make_shared<Buffer>(buf);
            Block ack_block((uint32_t)498, buf_p);
            ack_block.encode();

            wrapperout.insert(wrapperout.elements_end() ,ack_block);


            if(mode){ //TRUE = NOTIFY
              if(consumer_prefix == ""){
                std::cout << "ERR: No consumer prefix provided for current operation mode." << std::endl;

              }
              else{
                
                prefixes[id405] = consumer_prefix;
                wrapperout.insert(wrapperout.elements_end() ,wrapper.get(402));
                //Sum(buf_arg, args_length, id405); //below
              }
            }
            else{ // FALSE = POLL
              /*auto buf2 = Buffer(id.data(), id.size()); 
              auto buf_p2 = std::make_shared<Buffer>(buf2);
              Block cons_id((uint32_t)402, buf_p2);
              cons_id.encode();*/

              auto buf3 = Buffer(id405.data(), id405.size()); 
              auto buf_p3 = std::make_shared<Buffer>(buf3);
              Block prod_id((uint32_t)405, buf_p3);
              prod_id.encode();

              wrapperout.insert(wrapperout.elements_end() ,wrapper.get(402));//cons_id);
              wrapperout.insert(wrapperout.elements_end() ,prod_id);

              //Sum(buf_arg, args_length, id);
            }

            wrapperout.encode();
          }

          sendData(interest.getName(),wrapperout);

          if(mode){
            m_face.processEvents(ndn::time::milliseconds(-100),false);
            t1= std::thread(&Producer::Function, this, buf_arg, args_length, id405);
            //Function(buf_arg, args_length, id405);
            t1.join();

          }
          else{
            t1= std::thread(&Producer::Function, this,  buf_arg, args_length, id405);
            
          }
          break;
        }

        case 2001:{
          
          for ( auto e: wrapper.elements() )
          { std::cout << "  << element inside wrapper: " << e << std::endl;
            if (e.type() == 402){
              Block block_id = wrapper.get(402);
              //id_block.parse();
              id = std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size()); 
            }

            if(e.type() == 403){
              Block prefix = wrapper.get(403);
              //prefix.parse();
              consumer_prefix = std::string(reinterpret_cast<const char*>(prefix.value()), prefix.value_size()); 
            }
            if(e.type() == 404){
              Block mode_op = wrapper.get(404);
              //mode_op.parse();
              const char* aux = reinterpret_cast<const char*>(mode_op.value());
              mode = *reinterpret_cast<const bool*>(aux); //bool) aux;//*reinterpret_cast<bool>(aux);
              std::cout << ">> Operation mode: " << mode << std::endl;
            }
            if(e.type() == 499){
              Block err = wrapper.get(499);
              //err.parse();
              std::cout << "ERROR: " <<  std::string(reinterpret_cast<const char*>(err.value()), err.value_size())  << std::endl;
            }
          }
          if(id == ""){
            std::cout << "ERR: no consumer id (402) provided." << std::endl;
            //TODO send Block 499
          }
          else{

            std::string id405 = generate_id();
            ids[id405] = id;
            modes[id405] = mode;

            /*std::string prfx = interest.getName().toUri();
            size_t pos = 0;
            std::string delimiter = "/";
            std::vector<std::string> vec;
            std::string token;
            while ((pos = prfx.find(delimiter)) != std::string::npos) 
            {
                  token = prfx.substr(0, pos);
                  vec.push_back(token);
                  prfx.erase(0, pos + delimiter.length());
            }
            int in=0;*/
            std::string plugin="average";
            /*for(std::string token : vec){
                if(token == "computation"){
                    plugin=vec[in+1];
                    break;
                }
                in++;
            }*/
            std::cout << ">> Plugin: " << plugin << std::endl;
            plugins[id405]=plugin;

            Block wrapperout((uint32_t)2101);

            std::string ack = "ACK";
            auto buf = Buffer(ack.data(), ack.size()); 
            auto buf_p = std::make_shared<Buffer>(buf);
            Block ack_block((uint32_t)498, buf_p);
            ack_block.encode();

            wrapperout.insert(wrapperout.elements_end() ,ack_block);

            wrapperout.encode();

            sendData(interest.getName(),wrapperout);

            //if(mode){ //TRUE = NOTIFY
              if(consumer_prefix == ""){
                std::cout << "ERR: No consumer prefix provided for current operation mode." << std::endl;
              }
              else{
                prefixes[id405] = consumer_prefix;
                Block block_id = wrapper.get(402);

                Block wrapperoutInt((uint32_t)2200);

                //std::cout << "----- filling 2200: id405: "<< id405 << std::endl;
                auto buf3 = Buffer(id405.data(), id405.size()); 
                auto buf_p3 = std::make_shared<Buffer>(buf3);
                Block prod_id((uint32_t)405, buf_p3);
                prod_id.encode();

                wrapperoutInt.insert(wrapperoutInt.elements_end() ,block_id);
                wrapperoutInt.insert(wrapperoutInt.elements_end() ,prod_id);
                wrapperoutInt.encode();

                sendInterest(prefixes[id405], wrapperoutInt);
              }
            //}
            //else{ // FALSE = POLL

              /*Block block_id = wrapper.get(402);

              auto buf3 = Buffer(id405.data(), id405.size()); 
              auto buf_p3 = std::make_shared<Buffer>(buf3);
              Block prod_id((uint32_t)405, buf_p3);
              prod_id.encode();

              Block wrapperoutInt((uint32_t)2200);
              wrapperoutInt.insert(wrapperoutInt.elements_end() ,block_id);
              wrapperoutInt.insert(wrapperoutInt.elements_end() ,prod_id);
              wrapperoutInt.encode();

              sendInterest(consumer_prefix, wrapperoutInt);*/

              //Sum(buf_arg, args_length, id);
            //}

          }

          break;
        }
        
        case 2002: {
          std::string id405="";
          for ( auto e: wrapper.elements() )
          { 
            //std::cout << "element inside app params block : " << e << std::endl;
            if (e.type() == 405){ 
              Block id_block = wrapper.get(405);
              // buffer.parse();
              id405 =std::string(reinterpret_cast<const char*>(id_block.value()), id_block.value_size());
              
            }
            if(e.type() == 499){
              Block err = wrapper.get(499);
              //err.parse();
              std::cout << "ERROR: " << std::string(reinterpret_cast<const char*>(err.value()), err.value_size())  << std::endl;
            }
          }


          Block wrapperout((uint32_t)2102);
          if(id405 != ""){
            
            
            Block result = results[id405];
            wrapperout.insert(wrapperout.elements_end() ,result);
            wrapperout.insert(wrapperout.elements_end() ,wrapper.get(405));
          }

          else{
            std::cout << "ERROR: no id provided." << std::endl;
          }
          std::cout << "sending data with result 2102" << std::endl;
          wrapperout.encode();
          sendData(interest.getName(),wrapperout);

          break;
        }

        case 2003: {
            std::string id405="";
            for ( auto e: wrapper.elements() )
            { 
              //std::cout << "element inside app params block : " << e << std::endl;
              if (e.type() == 405){ 
                Block id_block = wrapper.get(405);
                // buffer.parse();
                id405 =std::string(reinterpret_cast<const char*>(id_block.value()), id_block.value_size());
                

              }
              if(e.type() == 499){
                Block err = wrapper.get(499);
                //err.parse();
                std::cout << "ERROR: " <<  std::string(reinterpret_cast<const char*>(err.value()), err.value_size())  << std::endl;
              }
            }
            Block wrapperout((uint32_t)2103);
            if(id405 != ""){
              
              check_result(id405);
              if( results.find(id405) == results.end() ){
                std::cout << ">> No output ready yet." << std::endl;
              }
              else{

                Block result = results[id405];
                wrapperout.insert(wrapperout.elements_end() ,result);
                t1.join();
              }

              /*auto buf3 = Buffer(id405.data(), id405.size()); 
              auto buf_p3 = std::make_shared<Buffer>(buf3);
              Block prod_id((uint32_t)405, buf_p3);
              prod_id.encode();*/

              wrapperout.insert(wrapperout.elements_end() ,wrapper.get(405));
            }

            else{
              std::cout << "ERROR: no id (405) provided." << std::endl;
            }

            wrapperout.encode();
            sendData(interest.getName(),wrapperout);

            break;
          }

          
         /*case : {
            Block in = appParams.get(type);
            in.parse();
            for ( auto e: in.elements())
            {
              if (e.type()==303){
                std::cout << "type 303 found: id" <<std::endl;
                Block block_id = in.get(303);
                id =std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size());
              }
            }
            if (id != ""){
              auto a_id = Buffer(id.data(), id.size());
              auto b_id = std::make_shared<Buffer>(a_id);
              Block id_block((uint32_t)303, b_id);
              id_block.encode();

              Block content((uint32_t)510);
              content.insert(content.elements_end() , id_block);
              content.insert(content.elements_end() , results[id]);
              content.encode(); 

              sendData(interest.getName(),content);
            }
            break;
          }*/
          default:{
           //json resp;
           std::cout << " unknown TLV type" << std::endl;
           //content=resp.dump();
           break;
         }
        }
    }
      
  }
  

    //static const std::string content("Hello, world!");

    // Create Data packet
    

    // in order for the consumer application to be able to validate the packet, you need to setup
    // the following keys:
    // 1. Generate example trust anchor
    //
    //         ndnsec key-gen /example
    //         ndnsec cert-dump -i /example > example-trust-anchor.cert
    //
    // 2. Create a key for the producer and sign it with the example trust anchor
    //
    //         ndnsec key-gen /example/testApp
    //         ndnsec sign-req /example/testApp | ndnsec cert-gen -s /example -i example | ndnsec cert-install -

    // Sign Data packet with default identity
    
  //***********************************************************
  //#     OnData                                            #
  //***********************************************************
  void
  onData(const Interest&, const Data& data)//, std::string prefix)
  {
    std::cout << ">> Data received: type ";
    Block response = data.getContent();
    response.parse();
    Block wrapper = response.elements()[0];
    wrapper.parse();
    int type = wrapper.type();
    std::cout << type << std::endl;

    std::string id="";
    //const unsigned char* buf_arg;
    std::string buf_arg;
    int args_length=0;

    switch(type){
      case 2300:{
        for ( auto e: wrapper.elements())
        {
          if(e.type()==405){
            Block block_id = wrapper.get(405);
              // buffer.parse();
            id =std::string(reinterpret_cast<const char*>(block_id.value()), block_id.value_size());
          }
          if(e.type()==300){
            std::cout<< "fetching input" << std::endl;
            Block data_block = wrapper.get(300);
            data_block.parse();

            for(auto e2 : data_block.elements() )
            {
              if(e2.type() ==400 ){
                
                Block buffer = data_block.get(400);
                
                //buf_arg = reinterpret_cast<const unsigned char*>(buffer.value());
                buf_arg=std::string(reinterpret_cast<const char*>(buffer.value()), buffer.value_size());
              }
              if(e2.type() ==401 ){
                
                Block length = data_block.get(401);
                //length.parse();
                const char* aux = reinterpret_cast<const char*>(length.value());
                args_length = *reinterpret_cast<const int*>(aux);
                
              } 
            }
          }
        }
        if(args_length != 0 && id!="" ){
          if(modes[id]){
            t1= std::thread(&Producer::Function, this, buf_arg, args_length, id);
            //Function(buf_arg, args_length, id405);
            t1.join();
          }
          else{
            t1= std::thread(&Producer::Function, this,  buf_arg, args_length, id);
          }
        }
          /*if (modes[id]){

            auto a2 = Buffer(id.data(),id.size());
            auto m2 = std::make_shared<Buffer>(a2);

            Block id_block((uint32_t)303,m2);
            id_block.encode();
            sendInterest(prefix, id_block, 133);
          }*/
         

        break;
      }

      case 2301:{ 
      for ( auto e: wrapper.elements())
        { if(e.type()==498){
            Block ack = wrapper.get(498);
              // buffer.parse();
            std::string ackn =std::string(reinterpret_cast<const char*>(ack.value()), ack.value_size());
          }
          if(e.type() == 499){
              auto c = wrapper.get(499);
              std::string err =std::string(reinterpret_cast<const char*>(c.value()), c.value_size());
              std::cout<<"ERR: " << err << std::endl;
            }
        }
        break;
      }


      case 521:{
        std::cout << "waiting to consumer get output.. "  << std::endl;
        break;
      }

      default:
      
        break;
      


    }

  }



  //***********************************************************
  //#     Generate id                                         #
  //***********************************************************
  std::string
  generate_id()
  {

    auto tme = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(tme);
    auto time = std::ctime(&end_time);
    std::hash<char*> ptr_hash;
    auto aux = (ptr_hash(time));

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string id = boost::lexical_cast<std::string>(uuid);

    id = id + std::to_string(aux);
    return id;
  }

  //***********************************************************
  //#     fetch input                                         #
  //***********************************************************

  std::string
  fetchInput(std::string prefix, std::string id){
    auto a2 = Buffer(id.data(),id.size());
    auto m2 = std::make_shared<Buffer>(a2);

    Block id_block((uint32_t)303,m2);
    id_block.encode();

    sendInterest(prefix, id_block);

  }

//***********************************************************
  //#     Sum function                                          #
  //***********************************************************
  
  void
  Function(std::string args, size_t size, std::string id){
    std::cout << "Running plugin\n\n";

    std::cout << "Name of plugin:" << plugins[id] << std::endl;
    std::string so = "./";
    so.append(plugins[id]);
    so.append(".so");
    void* handle = dlopen(so.c_str(), RTLD_LAZY);

    std::string error_m="";
    std::string result;
    
    if (!handle) {
        std::cerr << "Cannot open library: " << dlerror() << '\n';
        error_m="Cannot open library: cannot open shared object file";
        //error_m.append(dlerror());
        //exit(1);
    }
    else{ 
      // load the symbol
      std::cout << "Loading...\n";
      typedef std::string (*function_t)(std::string);

      // reset errors
      dlerror();
      function_t function = (function_t) dlsym(handle, "function");
      const char *dlsym_error = dlerror();
      if (dlsym_error) {
          std::cerr << "Cannot load symbol 'function': " << dlsym_error <<
              '\n';
          dlclose(handle);
          error_m="Cannot open library: ";
          error_m.append(dlsym_error);
          //exit(1) ;
      }
      else{
        // use it to do the calculation
        std::cout << "Calling function...\n";
        result = function(args);
        std::cout << "result: " << result << std::endl;
        
        // close the library
        std::cout << "Closing library...\n";
        dlclose(handle);
      }
    }
    
    if(error_m!=""){
      auto a = Buffer(error_m.data(),error_m.size());
      auto b = std::make_shared<Buffer>(a);
      Block res((uint32_t)499, b);
      res.encode();
      results[id] = res;
    }
    else{
      auto a = Buffer(result.data(),result.size());
      auto b = std::make_shared<Buffer>(a);
      Block res((uint32_t)301, b);
      res.encode();
      results[id] = res;
    }
    
    
    if (modes[id]){
        Block wrapper((uint32_t)2201);
        auto a = Buffer(id.data(),id.size());
        auto b = std::make_shared<Buffer>(a);
        Block id_p((uint32_t)405, b);
        id_p.encode();

        auto a2 = Buffer(ids[id].data(),ids[id].size());
        auto b2 = std::make_shared<Buffer>(a2);
        Block id_c((uint32_t)402, b2);
        id_c.encode();

        wrapper.insert(wrapper.elements_end() , id_c);
        wrapper.insert(wrapper.elements_end() , id_p);
        wrapper.encode();

        sendInterest(prefixes[id], wrapper);
    }

    
  }


  //***********************************************************
  //#     check files to get errors or ack                    #
  //***********************************************************

 void check_result(std::string id){
    std::string file_res = id + "_out.txt";
    std::string file_err = id + "_err.txt";
    std::string line;
    std::string err_msg="";
    std::string res_msg="";
    std::ifstream myfile (file_err);
    std::ifstream myfile2 (file_res);
    if (myfile.is_open())
    {
      while ( getline (myfile,line) )
      {
        err_msg = err_msg + line + "\n";
      }
      myfile.close();

    }
    if (myfile2.is_open())
    {
      while ( getline (myfile2,line) )
      {
        res_msg = res_msg + line + "\n";
      }
      myfile2.close();
      int status = remove(file_res.c_str());
      if(status==0)
          std::cout<<"\nFile with output Deleted Successfully! .." << std::endl;
    }
    else std::cout << "------ Unable to open files ----" << std::endl; 

    
    if (err_msg != "") {

      auto a = Buffer(err_msg.data(),err_msg.size());
      auto b = std::make_shared<Buffer>(a);
      Block res((uint32_t)499, b);
      res.encode();
      results[id] = res; 
    }
    else if (res_msg != "") {

      auto a = Buffer(res_msg.data(),res_msg.size());
      auto b = std::make_shared<Buffer>(a);
      Block res((uint32_t)301, b);
      res.encode();
      results[id] = res; 
    }
     
  }
  //***********************************************************
  //#     aux functions                                       #
  //***********************************************************
  

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

  void
  onRegisterFailed(const Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix '" << prefix
              << "' with the local forwarder (" << reason << ")" << std::endl;
    m_face.shutdown();
  }


  void
  sendInterest(std::string prefix, Block appParams/*, int type*/){

    
    Name interestName(prefix);
    interestName.appendVersion();
    Interest interest(interestName);


      interest.setApplicationParameters( appParams );
      interest.setMustBeFresh(true);
      interest.setInterestLifetime(10_s); // The default is 4 seconds

      std::cout << "<< Sending Interest of type " << appParams.type() << ": " << interest << std::endl;
      m_face.expressInterest(interest,
                             std::bind(&Producer::onData, this,  _1, _2),
                             std::bind(&Producer::onNack, this, _1, _2),
                             std::bind(&Producer::onTimeout, this, _1));

      // processEvents will block until the requested data is received or a timeout occurs
      //m_face.processEvents();
    

  }

  void
  sendData(const Name& prefix, Block content /*std::string content*/){
    auto data = make_shared<Data>(prefix);
    data->setFreshnessPeriod(10_s);
    data->setContent( content);//make_span(reinterpret_cast<const uint8_t*>(content.data()), content.size()));

    m_keyChain.sign(*data);
    // m_keyChain.sign(*data, signingByIdentity(<identityName>));
    // m_keyChain.sign(*data, signingByKey(<keyName>));
    // m_keyChain.sign(*data, signingByCertificate(<certName>));
    // m_keyChain.sign(*data, signingWithSha256());

    // Return Data packet to the requester
    std::cout << "<< D: " << content << std::endl;
    m_face.put(*data);
  }

private:
  Face m_face;
  KeyChain m_keyChain;
  ScopedRegisteredPrefixHandle m_certServeHandle;
  std::map<std::string, Block> results;
  std::map<std::string, std::string>  prefixes;
  std::map<std::string, std::string> ids;
  std::map<std::string,  bool> modes;
  std::map<std::string, std::string> plugins;

  std::fstream config_file;
  int port_serv;
  std::string host_serv;
  std::thread t1;
public:
  std::string my_pref;
  

};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{ 
  try {
    
    ndn::examples::Producer producer;
    producer.run();
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }
}
