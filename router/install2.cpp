#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include "pstream.h"
#include <string>
#include <chrono>

extern "C" std::string function(std::string args) {

  //std::cout << "ARGS: " << args << "\n";
  size_t pos=0;
  std::string delim="||";
  std::vector<std::string> vec;
  std::string token;
  while((pos=args.find(delim)) != std::string::npos) {
    token= args.substr(0,pos);
    vec.push_back(token);
    args.erase(0,pos+delim.length());
  }
  std::string image = vec[0];
  std::string options = vec[1];
  if(options.find("-d") == std::string::npos) {
    options.append(" -d");
  }
  //std::cout << "OPTS: " << options << "\n";
  std::chrono::steady_clock::time_point start1;
  std::string cmd = "sudo docker run ";
  cmd.append(options);
  cmd.append(" ");
  cmd.append(image);
  std::string check = "sudo docker images -q ";
  check.append(image);
  std::string pull = "sudo docker pull ";
  pull.append(image);

  /////////////////////////////////// CHECK and PULL ////////////////////////////
  redi::ipstream proc3(check, redi::pstreams::pstdout | redi::pstreams::pstderr);
  std::string line;
  std::string exists="";
  while (std::getline(proc3.out(), line))
  {  //std::cout << "stdout: " << line << '\n';
      exists.append(line);
  }
  // if reading stdout stopped at EOF then reset the state:
  if (proc3.eof() && proc3.fail())
  {  proc3.clear();
  }
  // read child's stderr
  
  if (exists == "") {
      start1 = std::chrono::steady_clock::now();
      redi::ipstream proc(pull, redi::pstreams::pstdout | redi::pstreams::pstderr);
      std::string line;
      std::string err="";
      // read child's stdout
      /*while (std::getline(proc.out(), line))
      {  //std::cout << "stdout: " << line << '\n';
      }*/
      // if reading stdout stopped at EOF then reset the state:
      /*if (proc.eof() && proc.fail())
      {  proc.clear();
      }*/
      // read child's stderr
      while (std::getline(proc.err(), line))
      {  //std::cout << "stderr: " << line << '\n';
         err.append(line);
      }
      if (err != "") {
        return err;
      }
  }
  else{
      start1 = std::chrono::steady_clock::now();
  }
  /////////////////////////////////// RUN ////////////////////////////
  redi::ipstream proc2(cmd, redi::pstreams::pstdout | redi::pstreams::pstderr);
  auto end1 = std::chrono::steady_clock::now();
  auto run_time= std::chrono::duration_cast<std::chrono::microseconds>(end1-start1).count();
  std::ofstream outfile;
  outfile.open("tests/run.txt", std::ios_base::app);
  outfile << run_time;
  //////////////////////////////////////////////////////////////////////////////
  //std::string line;
  std::string res="";
  std::string err="";
  // read child's stdout
  while (std::getline(proc2.out(), line))
  {  //std::cout << "stdout: " << line << '\n';
    res.append(line);
  }
  // if reading stdout stopped at EOF then reset the state:
  if (proc2.eof() && proc2.fail())
  {  proc2.clear();
  }
  // read child's stderr
  while (std::getline(proc2.err(), line))
  {  //std::cout << "stderr: " << line << '\n';
     err.append(line);
  }
  if (err != "") {
    res=err;
  }
  else{
    res.append(" container id");
  }
   

  return res;
  
    
  
}


