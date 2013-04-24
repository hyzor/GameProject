#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Package.h"

using boost::asio::ip::tcp;


void addSockets();

boost::asio::io_service io_service;
tcp::acceptor* acceptor;

std::vector<tcp::socket*> sockets;
int main()
{
	std::cout << "Port: ";
	int port;
	std::cin >> port;
	acceptor = new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port));

	std::cout << "Server running" << std::endl;
	boost::thread f(addSockets);
	f.join();


	delete acceptor;

	return 0;
}

void getSendPackade(tcp::socket *s);

void addSockets()
{
	while(true)
	{
		tcp::socket *s = new tcp::socket(io_service);
		acceptor->accept(*s);
		sockets.push_back(s);
		std::cout << s << " connected\n";
		boost::thread GetSendPackage(boost::bind(getSendPackade, s));
	}
}

void getSendPackade(tcp::socket *s)
{
	while(true)
	{
		char* buf = new char[256];
		boost::system::error_code error;
		size_t len = (*s).read_some(boost::asio::buffer(buf, 256), error);

		if (error == boost::asio::error::eof) //disconnect
		{
			delete [] buf;
			int at = 0;
			for(int i = 1; i < sockets.size(); i++)
				if(sockets[i] == s)
				{
					at = i;
					break;
				}
			std::cout << s << " disconnected\n";
			sockets.erase(sockets.begin()+at);
			break;
		}
		else //new package
		{
			Package p = Package(buf, len);

			std::cout << "Forwarding package from " << s << " :" << std::endl 
				<< "  operation: " << p.GetHeader().operation << std::endl
				<< "  id       : " << p.GetHeader().id << std::endl
				<< "  body size: " << p.GetHeader().contentsize << std::endl
				<< "  body     : ";
			for(int i = 0; i < p.GetHeader().contentsize; i++)
				std::cout << p.GetBody().data[i];
			std::cout << std::endl;


			for(int i = 0; i < sockets.size(); i++) //forward package
			{
				if(s != sockets[i]) 
				{
					boost::system::error_code ignored_error;
					boost::asio::write(*sockets[i], boost::asio::buffer(buf, len), boost::asio::transfer_all(), ignored_error);
				}
			}
		}
	}
}