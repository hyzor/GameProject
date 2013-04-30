#ifndef PACKAGE_H
#define PACKAGE_H

class Package
{
	public:
		class Body
		{
			public:
				Body();
				Body(char* data);

				char* data;
		};
		class Header
		{
			public:
				Header();
				Header(int operation, int id, int contentsize);

				int operation;
				int id;
				int contentsize;
		};

		Package();
		Package(char* data, int size);
		Package(Header header, Body body);
		~Package();

		char* GetData();
		int Size();		

		Body GetBody();
		Header GetHeader();

	private:
		char* data;
		int size;
		Header header;
		Body body;

		void setHeader();
		void setBody();
};



#endif