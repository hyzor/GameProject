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
tcp::socket* addsocket;

Game* game;
std::queue<PackageTo*>* send2;

int main()
{
	Python->Initialize();

	std::cout << "Port: ";
	int port = 13;
	//std::cin >> port;
	acceptor = new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port));

	send2 = new std::queue<PackageTo*>();
	game = new Game(send2);

	std::cout << "Server running" << std::endl;
	
	addsocket = NULL;
	boost::thread f(addSockets);
	sendPackage();

	delete acceptor;
	delete game;
	Python->ShutDown();
	return 0;
}


void addSockets()
{
	while(true)
	{
		tcp::socket *s = new tcp::socket(io_service);
		acceptor->accept(*s);
		addsocket = s;
		std::cout << s << " connected\n";
	}
}

void disconect(tcp::socket *s)
{
	int at = 0;
	for(unsigned int i = 1; i < sockets.size(); i++)
		if(sockets[i] == s)
		{
			at = i;
			break;
		}
	std::cout << s << " disconnected\n";
	boost::system::error_code error;
	(*s).shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
	(*s).close();
	delete s;
	sockets.erase(sockets.begin()+at);
	game->Disconnect((char*)s);
}
void sendPackage()
{
	while(true)
	{
		game->Update();
		
		if(addsocket != NULL)
		{
			sockets.push_back(addsocket);
			addsocket = NULL;
		}

		for(int i = 0; i < sockets.size(); i++)
		{
			boost::system::error_code error2;
			size_t len = (*sockets[i]).available(error2);
			if(len > 0)
			{
				char* buf = new char[len];

				boost::system::error_code error;
				len = (*sockets[i]).read_some(boost::asio::buffer(buf, len), error);

				if (error)
				{
					delete [] buf;
					disconect(sockets[i]);
					break;
				}
				else
				{
					int offset = 0;
					while(len-offset > 0)
					{
						Package p = Package((char*)(buf+offset), true);
						game->HandelPackage(&p, (char*)sockets[i]);
						offset += p.Size();
					}
				}

				delete [] buf;
			}
			if(error2)
			{
				disconect(sockets[i]);
				break;
			}

		}

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
						boost::system::error_code error;
						boost::asio::write(*sockets[i], boost::asio::buffer(p->GetData(), p->Size()), boost::asio::transfer_all(), error);
						if (error == boost::asio::error::eof)
							disconect(sockets[i]);
					}
				}
			}
			else
			{
				for(unsigned int i = 0; i < sockets.size(); i++)
				{
					if(to == (char*)sockets[i])
					{
						boost::system::error_code error;
						boost::asio::write(*sockets[i], boost::asio::buffer(p->GetData(), p->Size()), boost::asio::transfer_all(), error);
						if (error == boost::asio::error::eof)
							disconect(sockets[i]);
					}
				}
			}

			delete p;

			//boost::this_thread::sleep(boost::posix_time::milliseconds(500));
		}
	}
}