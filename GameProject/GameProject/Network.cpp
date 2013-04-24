#include "Network.h"

Network* Network::instance = 0;

Network* Network::GetInstance()
{
	if(!instance)
		instance = new Network();
	return instance;
}

Network::Network()
{
	this->Run = false;
	this->send = std::queue<Package*>();
}

Network::~Network()
{
	while(!Queue().empty())
			delete Pop();
}

void Network::Initialize()
{
	//this->Run = false;
}


void Network::Start()
{
	this->Run = true;
	
	resolver = new tcp::resolver(io_service);

	tcp::resolver::query query("localhost", "13");
	
	endpoint_iterator = resolver->resolve(query);
	tcp::resolver::iterator end;
	s = new tcp::socket(io_service);
	boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
		(*s).close();
		(*s).connect(*endpoint_iterator++, error);
    }
    if (error)
		close();
}

Package* Network::GetPackage()
{
	size_t len = (*s).available();
	if(len > 0)
	{
		char* buf = new char[len];

		boost::system::error_code error;
		len = (*s).read_some(boost::asio::buffer(buf, len), error);

		if (error == boost::asio::error::eof)
		{
			close();
			delete [] buf;
		}
		else
			return new Package(buf, len);
	}
	return new Package();
}

void Network::SendPackage(char* data, int size)
{
	boost::system::error_code error;
	boost::asio::write(*s, boost::asio::buffer(data, size), boost::asio::transfer_all(), error);
	if (error)
		Close();
}

void Network::Close()
{
	if(this->Run)
	{
		close();
		delete instance;
	}
}

void Network::close()
{
	this->Run = false;
	boost::system::error_code error;
	s->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
	s->close();
	delete s;
	delete resolver;
}

bool Network::Running()
{
	return Run;
}

std::queue<Package*> Network::Queue()
{
	return send;
}

void Network::Push(Package* p)
{
	send.push(p);
}

Package* Network::Pop()
{
	Package* p = send.front();
	send.pop();
	return p;
}