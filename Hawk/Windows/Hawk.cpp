#include<iostream>
#include<map>
#include<forward_list>
#include<string.h>
#include<windows.h>
#include<unistd.h>
using namespace std;
class validator
{
	private :
	validator(){}
	public : 
	static bool isValidMIMEType(string &mimeType)
	{
		//right now, do nothing
		return true;
	}
	static bool isValidPath(string &path)
	{
		//right now, do nothing
		return true;
	}
	static bool isValidURLFormat(string &url)
	{
		return true;
	}
};
class Error
{
	private :
	string error;
	public : 
	Error(string error)
	{
		this->error=error;
	}
	bool hasError()
	{
		return this->error.length()>0;
	}
	string getError()
	{
		return this->error;
	}
};
class Request
{
};
class Response
{
	private : 
	string contentType;
	forward_list<string> content;
	forward_list<string>::iterator contentIterator;
	unsigned long contentLength;
	public : 
	Response()
	{
		this->contentLength=0;
		this->contentIterator=this->content.before_begin();
	}
	~Response()
	{
 		//not yet decided
	}
	void setContentType(string contentType)
	{
		if(validator::isValidMIMEType(contentType))
		{
			this->contentType=contentType;
		}
	}
	Response & operator<<(string content)
	{
		this->contentLength+=content.length();
		this->contentIterator=this->content.insert_after(this->contentIterator,content);
		return *this;
	}
};
class Hawk
{
	private : 
	string staticResourceFolder;
	map<string,void (*)(Request &,Response &)> urlMappings;
	public : 
	void setStaticResourceFolder(string staticResourceFolder)
	{
		// do nothing right now
	}
	void get(string url,void (*callBack)(Request &,Response &))
	{
		if(validator::isValidURLFormat(url))
		{
			urlMappings.insert(pair<string,void (*)(Request &,Response &)>(url,callBack));
		}
	}
	void listen(int portNumber,void (*callBack)(Error &))
	{
		WSADATA wsaData;
		WORD ver;
		ver=MAKEWORD(1,1);
		WSAStartup(ver,&wsaData);
		int serverSocketDescriptor;
		char requestBuffer[4096];
		int requestLength;
		int x;
		serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(serverSocketDescriptor<0)
		{
			Error error("Unable to create socket");
			callBack(error);
			return;
		}
		struct sockaddr_in serverSocketInformation;
		serverSocketInformation.sin_family=AF_INET;
		serverSocketInformation.sin_port=htons(portNumber);
		serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
		int successCode=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
		if(successCode<0)
		{
			close(serverSocketDescriptor);
			WSACleanup();
			char a[101];
			sprintf(a,"unable to bind socket to port : %d",portNumber);
			Error error(a);
			callBack(error);
			return;
		}
		successCode=::listen(serverSocketDescriptor,10);
		if(successCode<0)
		{
			close(serverSocketDescriptor);
			WSACleanup();
			Error error("Unable to accept client connections.");
			callBack(error);
			return;
		}
		Error error("");
		callBack(error);
		struct sockaddr_in clientSocketInformation;
		int len=sizeof(clientSocketInformation);
		int clientSocketDescriptor;
		while(1)
		{
			clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation, &len);			
			if(clientSocketDescriptor<0)
			{
				//not yet decided, will write this code later on
			}
			requestLength=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer),0);
			if(requestLength>0)
			{
				for(x=0;x<requestLength;x++)printf("%c",requestBuffer[x]);
				const char * response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 143\r\n\r\n<html><head><title>ThinkingMachines</title></head><body><h1>Thinking Machines</h1><h3>We Teach more than we promise to teach</h3></body></html>";
				send(clientSocketDescriptor,response,strlen(response),0);
			}
		}//infinte loop ends here
	}
};
// Hawk [The web application developer - user of Hawk web server] 
int main()
{
	Hawk hawk;
	hawk.setStaticResourceFolder("Whatever");
	hawk.get("/",[](Request &request, Response &response) void {
		const char * html ="<!DOCTYPE HTML><html><meta charset='utf-8'><title>Whatever</title></head><body><h1>Welcome</h1><a href='getCustomers'>Customers List</a></body></html>";
		response.setContentType("text/html");// setting MIME Type)
		response<<html;
	});
	hawk.listen(6060,[](Error &error) void {
		if(error.hasError())
		{
			cout<<error.hasError();
			return;
		}
		cout<<"Hawk HTTp Server is ready to accept request on port 6060"<<endl;
	});
	return 0;
}