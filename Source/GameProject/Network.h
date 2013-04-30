#ifndef NETWORK_H
#define NETWORK_H

#include <boost/asio.hpp>
#include <queue>
#include "Package.h"

using boost::asio::ip::tcp;

class Network
{
	public:
		static Network* GetInstance();
		~Network();
		void Initialize();
		
		void Start();
		void Close();
		
		Package* GetPackage();
		void SendPackage(char* data, int size);
		std::queue<Package*> Queue();
		void Push(Package* p);
		Package* Pop();

		bool Running();
	private:
		static Network *instance;
		Network();

		void close();

		bool Run;

		tcp::socket *s;
		boost::asio::io_service io_service;
		tcp::resolver *resolver;
		tcp::resolver::iterator endpoint_iterator;
		
		std::queue<Package*> send;
};


#endif