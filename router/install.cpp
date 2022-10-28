#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
#include <fstream>
//using namespace std;

extern "C" std::string function(std::string args) {

  std::srand (static_cast <unsigned> (time(0)));
  int r3 = static_cast <int> (std::rand()); ///( static_cast <float> (RAND_MAX));

  std::string id = std::to_string(r3);
  std::cout << "ID: " << id << "\n";

  std::string shell = "./execute_function.sh"; 
  shell.append(" '");
  shell.append( args); 
  shell.append("'");
  shell.append(" '");
  shell.append(id);
  shell.append("'");
  std::cout << "shell: " << shell << "\n";
  system(shell.c_str());

  
  std::string file_err = id + "_err.txt";
  std::string file_out = id + "_out.txt";
  std::string line;

  std::ifstream myfile (file_err);
  std::ifstream myfile2 (file_out);

  std::string res="";
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      //std::cout<<  line << "\n";
      res += line + "\n";
    }
    myfile.close();
    int status = remove(file_err.c_str());
    if(status==0) {}
        //std::cout<<"\nFile with error Deleted Successfully!" << std::endl;

  }
  else if (myfile2.is_open())
  {
    while ( getline (myfile2,line) )
    {
      //std::cout<<  line << "\n";
      res += line + "\n";
    }
    myfile2.close();
    int status = remove(file_out.c_str());
    if(status==0) { }
        //std::cout<<"\nFile with output Deleted Successfully!" << std::endl;

  }
  else {
    std::cout<<  "NO FILE FOUND" << "\n";
  }

  /*float res;
  int i=0;
  size_t pos = 0;
  std::string delimiter = "||";
  //std::vector<std::string> vec;
  std::string img;
  std::string options;
  if ( (pos = args.find(delimiter)) != std::string::npos ){
    img = args.substr(0,pos);
    options = args.erase(0, pos + delimiter.length());
  }*/


  //std::cout << "ing: " << img << " / " << options << std::endl;
  /*while ((pos = sens.find(delimiter)) != std::string::npos) {
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

  return res;
}

