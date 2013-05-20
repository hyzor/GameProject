#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <queue>
#include "Package.h"
#include "Game.h"

using boost::asio::ip::tcp;


void addSockets();
void sendPackage();

boost::asio::io_service io_service;
tcp::acceptor* acceptor;

std::vector<tcp::socket*> sockets;

Game* game;
std::queue<PackageTo*>* send2;

int main()
{
	Python->Initialize();

	std::cout << "Port: ";
	int port;
	std::cin >> port;
	acceptor = new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port));

	send2 = new std::queue<PackageTo*>();
	game = new Game(send2);

	std::cout << "Server running" << std::endl;
	boost::thread f(addSockets);
	boost::thread SendPackage(sendPackage);
	f.join();

	delete acceptor;
	delete game;
	Python->ShutDown();
	return 0;
}

void getPackade(tcp::socket *s);

void addSockets()
{
	while(true)
	{
		tcp::socket *s = new tcp::socket(io_service);
		acceptor->accept(*s);
		sockets.push_back(s);
		std::cout << s << " connected\n";
		boost::thread GetPackage(boost::bind(getPackade, s));
	}
}

void getPackade(tcp::socket *s)
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
			for(unsigned int i = 1; i < sockets.size(); i++)
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
			game->HandelPackage(&p, (char*)s);
		}
	}
}

void sendPackage()
{
	while(true)
	{
		game->Update();

		while(!send2->empty())
		{
			Package* p = send2->front()->p;
			char* to = send2->front()->to;
			send2->pop();

			std::cout << "Forwarding package from " << p->GetHeader().id << " to " << (int)to << " :" << std::endl 
				<< "  operation: " << p->GetHeader().operation << std::endl
				<< "  id       : " << p->GetHeader().id << std::endl
				<< "  body size: " << p->GetHeader().contentsize << std::endl
				<< "  body     : ";
			for(int i = 0; i < p->GetHeader().contentsize; i++)
				std::cout << p->GetBody().data[i];
			std::cout << std::endl;

			if(to == 0)
			{
				for(unsigned int i = 0; i < sockets.size(); i++)
				{
					if(p->GetHeader().id != (int)sockets[i])
					{
						boost::system::error_code ignored_error;
						boost::asio::write(*sockets[i], boost::asio::buffer(p->GetData(), p->Size()), boost::asio::transfer_all(), ignored_error);
					}
				}
			}
			else
			{
				for(unsigned int i = 0; i < sockets.size(); i++)
				{
					if(to == (char*)sockets[i])
					{
						boost::system::error_code ignored_error;
						boost::asio::write(*sockets[i], boost::asio::buffer(p->GetData(), p->Size()), boost::asio::transfer_all(), ignored_error);
					}
				}
			}

			delete p;
		}
	}
}