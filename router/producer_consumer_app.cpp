
#include <iostream>
#include "producer_consumer.hpp"
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

	std::cout << "##### instantiate my function on router. #####" << std::endl;

    std::string plugin="install2";

	std::string img="alexkvlad/ndn-test:producer_average2";

	std::string opts="--rm --name avr --network='host'";


	std::string args;
	args.append(img);
	args.append("||");
	args.append(opts);
	args.append("||");
	args.append("/producer/computation/");

	int b;
	std::cout << "Choose mode: 0 Notify, 1 Polling" << std::endl;
	std::cin >> b ;
    bool mode;

	if(b==0){
		mode = true;
	}
	else{
		mode = false;
	}
	std::cout << "ARGs >>" << args << "\n";

	ndn::examples::Consumer consumer; 
	std::string pref = "/router/execute/";
	pref.append(plugin);
	std::string id = consumer.functionPrefix(pref);
	std::cout << "-> consumer id: " << id << std::endl;
	consumer.setArguments(id, args, mode);

	consumer.execute(id, mode);
	
	while (consumer.getResponse(id) == "" ) {
		std::cout << "waiting..." << std::endl;
		//sleep(100);
	}
	std::string result = consumer.getResponse(id);

	std::cout << "\n Result : " << result << std::endl;

}