#include"socketHTTP.h"
#include<iostream>
#include<fstream>

#define PORT_FILE "port.ini"

int get_port() {
	int port = 37020;
	ifstream ifs(PORT_FILE);
	if (ifs.good())
	{
		ifs >> port;
		ifs.close();
		return port;
	}
	else
	{
		ofstream ofs(PORT_FILE);
		ofs << port;
		ifs.close();
		ofs.close();
		return port;
	}
}

int main() {
	int port = get_port();
	std::cout << "将绑定端口(可自行在port.ini中修改):"<<port<<endl;
	HttpServer new_srv("0.0.0.0", port);
	new_srv.start();
}