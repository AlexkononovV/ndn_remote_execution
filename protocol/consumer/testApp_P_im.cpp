
#include <iostream>
#include "consumer_simple.hpp"
#include <stdlib.h> 

struct Msg {
	
	std::string function_prefix;
	std::string content;


};


class testApp {
	
	//std::cout << "    ..." << std::endl;
};

int 
main(int argc, char** argv){

	std::cout << "##### Get average/sum temperature: #####" << std::endl;
	//std::cout << "Name of plugin:" << std::endl;
    std::string plugin = "average";
    bool mode;
    /*std::getline(std::cin, plugin);

	std::cout << "##### Write ndn prefixes of sensors ( ex.: /s1 /it/s2 end)" << std::endl;
	std::string a;
	std::string args = ""; //"avr:";
	//std::vector<std::string> args;
	
	do {
		std::cin >> a ;
		if( a == "end"){
			break;
		}
		else{
			args.append(a);
			args.append(std::string(","));
		}
	}
	while (true);
	std::cout << "ARGS: " << args << std::endl;
	*/

    /*
	int b;
	std::cout << "Choose mode: 0 Notify, 1 Polling" << std::endl;
	std::cin >> b ;

	if(b==0){
		mode = true;
	}
	else{
		mode = false;
	}*/
	mode=false;


	ndn::examples::Consumer consumer; //();
	std::string pref = "/producer/computation/";
	pref.append(plugin);
	std::string id = consumer.functionPrefix(pref);
	std::cout << "-> consumer id: " << id << std::endl;
	//consumer.setArguments(id, bytes, length, mode);
	consumer.setArguments(id, "/iot/temp/1,/iot/temp/1,/iot/temp/1,/iot/temp/2,/iot/temp/3,/iot/temp/4,/iot/temp/2,", mode);

	consumer.execute(id,mode);
	std::cout << "### AFTER EXECUTE testapp , getting result" << "\n";
	while (consumer.getResponse(id) == "" ) {
		std::cout << "waiting..." << std::endl;
		sleep(1);
	}
	std::string result = consumer.getResponse(id);

	std::cout << "\n Result = " << result << std::endl;

}