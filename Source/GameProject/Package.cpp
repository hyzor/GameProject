#include "Package.h"


Package::Package()
{
	this->size = sizeof(Package::Header);
	this->data = new char();
}

Package::Package(char* data, int size)
{
	this->data = data;
	this->size = sizeof(Package::Header)+size;

	setHeader();
	setBody();
}

Package::Package(Package::Header header, Package::Body body)
{
	size = sizeof(Package::Header)+header.contentsize;
	data = new char[size];
	Package::Header* h = (Package::Header*)data;
	h->operation;
	*h = header;
	for(int i = 0; i < h->contentsize; i++)
		*(data+sizeof(Package::Header)+i) = *(body.data+i);
	delete [] body.data;

	setHeader();
	setBody();
}

Package::~Package()
{
	delete [] data;
}

char* Package::GetData()
{
	return data;
}

int Package::Size()
{
	return size;
}

Package::Header Package::GetHeader()
{
	return header;
}

Package::Body Package::GetBody()
{
	return body;
}

void Package::setHeader()
{
	char* offset = data;
	header.operation = *offset;
	offset += 4;
	header.id = *((int*)offset);
	offset += 4;
	header.contentsize = *((int*)offset);
}

void Package::setBody()
{
	body.data = (data+sizeof(Header));
}

void Package::SetId(int id)
{
	Header* h = (Header*)data;
	h->id = id;
	header.id = h->id;
}


Package::Body::Body(){From(0);}
Package::Body::Body(char* data)
{
	this->data = data;
	From(0);
}

void Package::Body::From(int at)
{
	this->readAt = at;
}

char* Package::Body::Read(int size)
{
	char* ret = data + readAt;
	readAt += size;
	return ret;
}

Package::Header::Header(){}
Package::Header::Header(int operation, int id, int contentsize)
{
	this->operation = operation;
	this->id = id;
	this->contentsize = contentsize;
}

