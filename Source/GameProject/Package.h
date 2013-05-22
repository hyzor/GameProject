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

				void From(int at);
				char* Read(int size);

				char* data;
			private:
				int readAt;
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
		Package(char* data);
		Package(char* data, bool shared);
		Package(Header header, Body body);
		~Package();

		char* GetData();
		int Size();
		void SetId(int id);

		Body GetBody();
		Header GetHeader();

	private:
		char* data;
		int size;
		Header header;
		Body body;
		bool shared;

		void setHeader();
		void setBody();
};

class PackageTo
{
	public:
		PackageTo(Package* p, char* to)
		{
			this->p = p;
			this->to = to;
		};

		Package* p;
		char* to;
};



#endif