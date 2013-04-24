#include "Package.h"


Package::Package()
{
	this->size = 0;
	this->data = new char();
}

Package::Package(char* data, int size)
{
	this->data = data;
	this->size = size;

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



Package::Body::Body(){}
Package::Header::Header(){}

Package::Body::Body(char* data)
{
	this->data = data;
}

Package::Header::Header(int operation, int id, int contentsize)
{
	this->operation = operation;
	this->id = id;
	this->contentsize = contentsize;
}

