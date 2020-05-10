#include <string>
#include <iostream>
#include <Script.hpp>

int main(){
	Script script;
	script.addFunction(test, "cfun");
	script.load("test.lua");

	std::cout << script.get<int>("int") << "\n";
	std::cout << script.get<unsigned>("unsigned") << "\n";
	std::cout << script.get<double>("double") << "\n";
	std::cout << script.get<bool>("bool") << "\n";
	std::cout << script.get<std::string>("string") << "\n";

	std::cout << script.get<std::string>("vec[2]") << "\n";
	std::cout << script.get<int>("vec", 3) << "\n";
	std::cout << script.get<int>("vec[1].v1") << "\n";

	script.openTable("vec");

	std::cout << script.get<float>(4) << "\n";

	script.openTable("[1].v3");

	for(int i = 1; i <= 5; i++)
	  std::cout << script.get<int>(i) << " ";
	std::cout << "\n";

	script.closeTable();

	std::cout << script.get<std::string>("v2") << "\n";

	for(int i = 1; i <= 4; i++)
	  std::cout << script.get<std::string>("v4", i) << " ";
	std::cout << "\n";

	script.closeTable(2);

	script.callFunction<1>("write", std::string("some text"));
	script.callFunction<1>("write", 5842);
	//script.callFunction<1>("write", "some text");  -- error -- "some text" is not a std::string
	std::cout << script.callFunction<int, 2>("sum", 102, -124) << "\n";
script.callFunction("callCFun");
}
